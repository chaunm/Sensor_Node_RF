#include "types.h"
#include "system.h"
#include "uart.h"
#include "flash.h"
#include "partition.h"
#include "ota.h"
#include "stwd100.h"
#include "device.h"
#include "devicedef.h"

static BYTE flashBuffer[40];
static DWORD firmwareKey[2] = {0x55AAAA55, 0x2112FEEF};
static DWORD writeAddress;
static BOOL flashFailed = FALSE;
static FLASHRESPONSE responseMessage;


VOID Ota_ProcessFlashPackage(PUARTBUFFER pBuffer)
{
    WORD i;
    PFLASHPACKAGE pFlash = (PFLASHPACKAGE)pBuffer->pData;
    // do not accept this kind of package
    if (((pFlash->nLength % 4) != 0) && (pFlash->flag != LAST_BLOCK))
    {
        flashFailed = TRUE; // will use to discard all later incoming message
        // send flash failed response & return
        responseMessage.nLength = FLASH_RESPONSE_LENGTH;
        responseMessage.nType = PACKAGE_TYPE_DEVICE_FLASH;
        responseMessage.nAddr = GetDeviceAddress();
        responseMessage.status = FLASH_FAILED;
        WriteUartIndirect(UART_PORT_1, (PUARTBUFFER)&responseMessage);
        return;
    }

    if ((flashFailed == TRUE) && (pFlash->flag != FIRST_BLOCK))
    {
        // send failed message & return
        responseMessage.nLength = FLASH_RESPONSE_LENGTH;
        responseMessage.nType = PACKAGE_TYPE_DEVICE_FLASH;
        responseMessage.nAddr = GetDeviceAddress();
        responseMessage.status = FLASH_FAILED;
        WriteUartIndirect(UART_PORT_1, (PUARTBUFFER)&responseMessage);
        return;     
    }

    // length valid
    if (pFlash->flag = FIRST_BLOCK)
    {
        flashFailed = FALSE;
        //prepare flash for writing data
        for (i = IMAGE_START_PAGE; i <= IMAGE_END_PAGE; i++)
        {
            EraseFlash(i, 1);
            // reset watchdog to avoid unwanted reset
            StwdManualToggle();
        }
        writeAddress = IMAGE_START_ADDR;
        // write first block -- write address is block address
        WriteFlash(pFlash->pData, pFlash->nLength, writeAddress >> 2);
        writeAddress += pFlash->nLength;
    }
    else if (pFlash->flag == LAST_BLOCK)
    {
        if ((pFlash->nLength % 4) == 0)
        {
            WriteFlash(pFlash->pData, pFlash->nLength, writeAddress >> 2);
        }
        else
        {
            CopyMemory(flashBuffer, pFlash->pData, pFlash->nLength);
            for (i = pFlash->nLength; i < 40; i++)
            {
                flashBuffer[i] = 0xFF;   
            }
            WriteFlash(flashBuffer, 40, writeAddress >> 2);
        }
        StwdManualToggle();
        // all data received - cleare information page, 
        for (i = INFORMATION_START_PAGE; i <= INFORMATION_END_PAGE; i++)
        {
            EraseFlash(i, 1);
            StwdManualToggle();
        }
        // write firmware key and reboot
        WriteFlash((PVOID)firmwareKey, sizeof(firmwareKey), INFORMATION_START_ADDR >> 2);
        responseMessage.nLength = FLASH_RESPONSE_LENGTH;
        responseMessage.nType = PACKAGE_TYPE_DEVICE_FLASH;
        responseMessage.nAddr = GetDeviceAddress();
        responseMessage.status = FLASH_SUCCESS;
        WriteUartIndirect(UART_PORT_1, (PUARTBUFFER)&responseMessage);
        Reboot();
    }
    else 
    {
        WriteFlash(pFlash->pData, pFlash->nLength, writeAddress >> 2);
        writeAddress += pFlash->nLength;
        StwdManualToggle();
    }
}
