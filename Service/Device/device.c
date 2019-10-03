/********************************************************************************
Bkav
Product: Platform STM32F051
Module: device.c
Version: 1.0
Author: ThiVD
Created: 4/2015
Modified:
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "uart.h"
#include "timer.h"
#include "stm32f0xx_flash.h"
#include "parameter.h"
#include "system.h"
#include "string.h"
#include "devicedef.h"
#include "device.h"
/*-----------------------------------------------------------------------------*/
/* Local Macro definitions */
/*-----------------------------------------------------------------------------*/
#define REGISTER_COUNT                  50
#define REPORT_REGISTER_INTERVAL        1000
#define UPDATE_BUFFER_INTERVAL          100

#define MAX_BUFFER_SIZE                 48
#define MAX_REGISTER_PART               8
#define MAX_PARAMETERS_PART             4

#define MAX_QUEUE_ZISE                  4


#define INSTRUCTION_REGISTER            0x01
#define NOOP_INSTRUCTION                0x00
#define RESET_INSTRUCTION               0x01
#define REBOOT_INSTRUCTION              0x02
#define REGISTER_INSTRUCTION            0x03
#define PARAMETER_INSTRUCTION           0x04
#define DEFINITION_INSTRUCTION          0x05
#define REG_UPDATE_INSTRUCTION          0x06
#define PARAM_UPDATE_INTRUCTION         0x07

#define PACKAGE_TYPE_DEFINITION_START   0x40
#define PACKAGE_TYPE_DEFINITION_DATA    0x0C
#define PACKAGE_TYPE_DEFINITION_ACK     0x41
#define PACKAGE_TYPE_DEFINITION_FINISH  0x42
/*-----------------------------------------------------------------------------*/
/* Local Data type definitions */
/*-----------------------------------------------------------------------------*/
typedef struct tagDEVICEBUFFER
{
    BYTE nLength;
    BYTE nType;
    WORD nAddr;
    BYTE pData[MAX_BUFFER_SIZE];
} DEVICEBUFFER, *PDEVICEBUFFER;
/*-----------------------------------------------------------------------------*/
/* Local Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables */
/*-----------------------------------------------------------------------------*/
INTERNAL char* g_pDefinition = NULL;
INTERNAL BOOL g_bIsConnected = FALSE;
INTERNAL BOOL g_bTickFlag = FALSE;

INTERNAL BYTE g_pRegisters[REGISTER_COUNT] = { 0 };
PBYTE REGISTERS = g_pRegisters;
INTERNAL PREGISTEREVENT g_pRegisterEvent = NULL;
INTERNAL BYTE g_nRegisterValue = 0;
INTERNAL DEVICEREGISTERPART g_pPendingRegisters[MAX_REGISTER_PART] = { 0 };
INTERNAL BYTE g_nPendingRegisterCount = 0;

INTERNAL DEVICEBUFFER g_arBufferDevice[MAX_QUEUE_ZISE] = { 0 };
INTERNAL BYTE g_nPendingBufferCount = 0;

INTERNAL BYTE g_nMaxPackageDefinition = 0;


INTERNAL SYSTEMCALLBACK g_pDeviceEvents[DEVICE_EVENT_COUNT] = { NULL };
/*-----------------------------------------------------------------------------*/
/* Function prototypes */
/*-----------------------------------------------------------------------------*/
INTERNAL VOID RequireRegisterInstruction(PVOID pData);
INTERNAL VOID PopulateRegisters();
INTERNAL VOID OnUartDataProc(PVOID pData);
INTERNAL VOID ProcessInstruction(PREGISTEREVENT pEvent);
INTERNAL VOID ProcessCustomRegister(PREGISTEREVENT pEvent);
INTERNAL VOID PopulateParameters();
INTERNAL VOID AppendRegister(BYTE nRegsiter, BYTE nValue);
INTERNAL VOID UpdateRegisterProc(PVOID pData);
INTERNAL VOID AllocatedBuffer(BYTE nType, PVOID pData, BYTE nLength);
INTERNAL VOID ReleaseBuffer();
INTERNAL VOID ProcessBufferDevice(PVOID pData);

/*-----------------------------------------------------------------------------*/
/* Function implementations */
/*-----------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------
Function:  VOID InitDevice()
Purpose: Initiazing the devide
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID InitDevice(PVOID pDefinition)
{
    WORD nLength = 0;
    g_pDefinition = (char*)pDefinition;
    RegisterUartCallback(UART_DATA_EVENT, OnUartDataProc);

    for (BYTE nIndex = 0; nIndex < REGISTER_COUNT; nIndex++)
    {
        g_pRegisters[nIndex] = INVALID_REGISTER_VALUE;
    }

    if (g_pDefinition != NULL)
    {
	  	nLength = strlen(g_pDefinition);
        g_nMaxPackageDefinition = nLength / MAX_BUFFER_SIZE;
        if ((nLength % MAX_BUFFER_SIZE) != 0)
        {
            g_nMaxPackageDefinition += 1;
        }
    }
    StartShortTimer(UPDATE_BUFFER_INTERVAL, UpdateRegisterProc, NULL); 
    StartShortTimer(UPDATE_BUFFER_INTERVAL, ProcessBufferDevice, NULL);
	StartShortTimer(REPORT_REGISTER_INTERVAL, RequireRegisterInstruction, NULL); // no need - chau nguyen
}
/*-------------------------------------------------------------------------------
Function:  VOID RegisterDeviceCallback(BYTE nEvent, SYSTEMCALLBACK fnDeviceCallback)
Purpose: Register the device callback function
Parameters: BYTE nEvent, SYSTEMCALLBACK fnDeviceCallback
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID  RegisterDeviceCallback(BYTE nEvent, SYSTEMCALLBACK fnDeviceCallback)
{
    if (nEvent < DEVICE_EVENT_COUNT)
    {
        g_pDeviceEvents[nEvent] = fnDeviceCallback;
    }
}
/*-------------------------------------------------------------------------------
Function:  INTERNAL VOID ResetDevice()
Purpose: reset the device
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID ResetDevice()
{
    ResetParams();
    StartShortTimer(500, Reboot, NULL);
}
/*-------------------------------------------------------------------------------
Function:  IsFlashing()
Purpose:
Parameters:
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
BOOL IsFlashing()
{
    return TRUE;
}
/*-------------------------------------------------------------------------------
Function:  SetPriority(BYTE nPriority)
Purpose:
Parameters:
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID SetPriority(BYTE nPriority)
{
    //
}
/*-------------------------------------------------------------------------------
Function:  SetDelay(WORD nDelay)
Purpose:
Parameters:
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID SetDelay(WORD nDelay)
{
    //
}
/*-------------------------------------------------------------------------------
Function:  VOID SetRegister(BYTE nRegister, BYTE nValue)
Purpose: Set the registers
Parameters: BYTE nRegister, BYTE nValue
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID SetRegister(BYTE nRegister, BYTE nValue)
{
    g_pRegisters[nRegister] = nValue;
	AppendRegister(nRegister, nValue);
}
/*-------------------------------------------------------------------------------
Function:  VOID RestoreRegister()
Purpose: Restore the register
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID RestoreRegister()
{
    if ((g_pRegisterEvent != NULL) && (g_pRegisterEvent->nValue != CURRENT_REGISTER_VALUE))
    {
        g_pRegisters[g_pRegisterEvent->nRegister] = g_nRegisterValue;
        AppendRegister(g_pRegisterEvent->nRegister, INVALID_REGISTER_VALUE);
        g_pRegisterEvent = NULL;
    }
}
/*-------------------------------------------------------------------------------
Function: BOOL IsConnected()
Purpose: Process the parameters buffer
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
BOOL IsConnected()
{
    return g_bIsConnected;
}
/*-------------------------------------------------------------------------------
Function: GetTickFlag
Purpose: Get Tick Flag from wireless module
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
BOOL GetTickFlag()
{
    return g_bTickFlag;
}
/*-------------------------------------------------------------------------------
Function: GetTickFlag
Purpose: Get Tick Flag
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
VOID ClearTickFlag()
{
    g_bTickFlag = FALSE;
}
/*-------------------------------------------------------------------------------
Function: HoldResetWifiModule
Purpose: Reset module wifi method
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
To send command reset wifi module SendCommand(PAKAGE_TYPE_WIFI_COMMAND, RESET_WIFI_MODULE);
To send command reflash wifi module SendCommand(PAKAGE_TYPE_WIFI_COMMAND, REFLASH_WIFI_MODULE);
To send command get tick from Zigbee module SendCommand(PACKAGE_TYPE_GET_TICK, 0x00);
To send command reboot wireless module SendCommand(PACKAGE_TYPE_REBOOT_WIRELESS, 0x00);
--------------------------------------------------------------------------------*/
VOID SendCommand(BYTE nType, BYTE nCommand)
{
//    UARTBUFFER stUartBuffer = { 0 };
//    BYTE arData[1] = { nCommand };
//    stUartBuffer = (UARTBUFFER){1, nType, ROOT, arData};
//    WriteUart(UART_PORT_2, (PVOID)&stUartBuffer, 1);
    AllocatedBuffer(nType, &nCommand, 1);
}
/*------------------------------------------------------------------------------- 
Function: RequireRegisterInstruction
Purpose: Require Register Instruction
Parameters: None
Return: None
Comments:  None
Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
INTERNAL VOID RequireRegisterInstruction(PVOID pData)
{
    SendCommand(PACKAGE_TYPE_REQUIRE_REGISTTER, 0x00);
}
/*-------------------------------------------------------------------------------
Function: PopulateRegisters()
Purpose: Populate the registers
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID PopulateRegisters()
{
    for (BYTE nIndex = 0; nIndex < REGISTER_COUNT; nIndex++)
    {
        if (g_pRegisters[nIndex] != INVALID_REGISTER_VALUE)
        {
            AppendRegister(nIndex, g_pRegisters[nIndex]);
        }
    }
}
/*-------------------------------------------------------------------------------
Function:  INTERNAL VOID PopulateParameters()
Purpose: Populate the parameters
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID OnUartDataProc(PVOID pData)
{
    PUARTBUFFER pBuffer = (PUARTBUFFER) pData;
    PBYTE pDataRemote  = (PBYTE)(pBuffer->pData);

    switch (pBuffer->nType)
    {
    case PACKAGE_TYPE_DEVICE_REGISTER:
        {
            BYTE nCount = pBuffer->nLength / sizeof(DEVICEREGISTERPART);
            for (BYTE nIndex = 0; nIndex < nCount; nIndex++)
            {
                REGISTEREVENT stEvent = {
					DEVICE_REGISTER_EVENT,
					_PDEVICEREGISTER(pDataRemote)->pPart[nIndex].nRegister,          // index of register
					_PDEVICEREGISTER(pDataRemote)->pPart[nIndex].nValue,             // new value register
					g_pRegisters[_PDEVICEREGISTER(pDataRemote)->pPart[nIndex].nRegister]  //old value of register
				};

                if (stEvent.nRegister == INSTRUCTION_REGISTER)
                    ProcessInstruction(&stEvent);
                else
                    ProcessCustomRegister(&stEvent);
            }
        }
        break;

    case PACKAGE_TYPE_DEVICE_PARAM:
        {
            BYTE nCount = pBuffer->nLength / sizeof(DEVICEPARAMPART);
            for (BYTE nIndex = 0; nIndex < nCount; nIndex++)
            {
                PARAMEVENT stEvent ={
                    DEVICE_PARAM_EVENT,
                    _PDEVICEPARAM(pDataRemote)->pPart[nIndex].nParam,
                    _PDEVICEPARAM(pDataRemote)->pPart[nIndex].nValue
                };

                if (g_pDeviceEvents[DEVICE_PARAM_EVENT] != NULL)
                {
                    g_pDeviceEvents[DEVICE_PARAM_EVENT](&stEvent);
                }

                if (stEvent.nValue != INVALID_PARAMETER_VALUE)
                {
                    if (!SetParam(stEvent.nParam, stEvent.nValue))
                        stEvent.nValue = INVALID_PARAMETER_VALUE;
                }

                DEVICEPARAMPART stDeviceParamPart;
                stDeviceParamPart.nParam = stEvent.nParam;
                stDeviceParamPart.nValue = stEvent.nValue;
                AllocatedBuffer(PACKAGE_TYPE_DEVICE_PARAM, &stDeviceParamPart, sizeof(DEVICEPARAMPART));
            }
        }
        break;


    case PACKAGE_TYPE_GET_TICK:
        g_bTickFlag = TRUE;
        break;

    case PACKAGE_TYPE_LOST_NETWORK:
        g_bIsConnected = FALSE;
        break;

 	case PACKAGE_TYPE_DEVICE_FLASH: // need to change here to process flash write  - chau nguyen
		FLASH_Unlock();
		FLASH_ProgramHalfWord(APPLICATION_ADDRESS, 0x00);
		FLASH_Lock();
//		ResetParams();
//		StartShortTimer(500, Reboot, NULL);
//		ResetDevice();
		break;
//#endif

    default:
        break;
    }
}
/*-------------------------------------------------------------------------------
Function:  INTERNAL VOID ProcessInstruction(PREGISTEREVENT pEvent)
Purpose: Process the instruction
Parameters: PREGISTEREVENT pEvent
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID ProcessInstruction(PREGISTEREVENT pEvent)
{
    switch (pEvent->nValue)
    {
    case NOOP_INSTRUCTION:
        break;

    case RESET_INSTRUCTION:
        ResetDevice();
        break;

    case REBOOT_INSTRUCTION:
        Reboot();
        break;

    case REGISTER_INSTRUCTION:
        PopulateRegisters();
        g_bIsConnected = TRUE;
        break;

    case PARAMETER_INSTRUCTION:
        PopulateParameters();
        g_bIsConnected = TRUE;
        break;

    default:
        ProcessCustomRegister(pEvent);
        break;
    }
}
/*-------------------------------------------------------------------------------
Function:  INTERNAL VOID ProcessCustomRegister(PREGISTEREVENT pEvent)
Purpose: Process the custom registers
Parameters: PREGISTEREVENT pEvent
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID ProcessCustomRegister(PREGISTEREVENT pEvent)
{
    g_pRegisterEvent = pEvent;
    if (g_pRegisterEvent->nValue != CURRENT_REGISTER_VALUE)
    {
        g_nRegisterValue = g_pRegisters[g_pRegisterEvent->nRegister];
        g_pRegisters[g_pRegisterEvent->nRegister] = g_pRegisterEvent->nValue;
    }

    if (g_pDeviceEvents[DEVICE_REGISTER_EVENT] != NULL)
        g_pDeviceEvents[DEVICE_REGISTER_EVENT](g_pRegisterEvent);

    if (g_pRegisterEvent != NULL)
    {
        AppendRegister(g_pRegisterEvent->nRegister, g_pRegisters[g_pRegisterEvent->nRegister]);
        g_pRegisterEvent = NULL;
    }
}
/*-------------------------------------------------------------------------------
Function:  INTERNAL VOID PopulateParameters()
Purpose: Populate the parameters
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID PopulateParameters()
{
    BYTE nCount = 0;
    DEVICEPARAMPART arDeviceParamPart[MAX_PARAMETERS_PART];

    for (BYTE nIndex = 0; nIndex < GetParamCount(); nIndex++)
    {
        if (PARAM_ITEMS[nIndex]->nParam != 0)
        {
            arDeviceParamPart[nCount].nParam = PARAM_ITEMS[nIndex]->nParam;
            arDeviceParamPart[nCount].nValue = PARAM_ITEMS[nIndex]->nValue;
            nCount++;
            if (nCount == MAX_PARAMETERS_PART)
            {
                AllocatedBuffer(PACKAGE_TYPE_DEVICE_PARAM, arDeviceParamPart, \
                    nCount * sizeof(DEVICEPARAMPART));
                nCount = 0;
            }
        }
    }
    if (nCount != 0)
    {
        AllocatedBuffer(PACKAGE_TYPE_DEVICE_PARAM, arDeviceParamPart, \
            nCount * sizeof(DEVICEPARAMPART));
    }
}
/*-------------------------------------------------------------------------------
Function:  AppendRegister
Purpose: Process the registers buffer
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID AppendRegister(BYTE nRegister, BYTE nValue)
{
    g_pPendingRegisters[g_nPendingRegisterCount].nRegister = nRegister;
	g_pPendingRegisters[g_nPendingRegisterCount].nValue = nValue;
	g_nPendingRegisterCount++;

	if (g_nPendingRegisterCount == MAX_REGISTER_PART)
		UpdateRegisterProc(NULL);
}
/*-------------------------------------------------------------------------------
Function:  UpdateRegisterProc
Purpose: Process the registers buffer
Parameters: None
Return: None
Comments:  None
Modified:
<Modified by>
<Date>
<Change>
--------------------------------------------------------------------------------*/
INTERNAL VOID UpdateRegisterProc(PVOID pData)
{
    if (g_nPendingRegisterCount != 0)
    {
        AllocatedBuffer(PACKAGE_TYPE_DEVICE_REGISTER, g_pPendingRegisters, \
            g_nPendingRegisterCount * sizeof(DEVICEREGISTERPART));
        g_nPendingRegisterCount = 0;
    }
    StartShortTimer(UPDATE_BUFFER_INTERVAL, UpdateRegisterProc, NULL);
}

INTERNAL VOID AllocatedBuffer(BYTE nType, PVOID pData, BYTE nLength)
{
    PBYTE pBuffer = (PBYTE)pData;

    g_arBufferDevice[g_nPendingBufferCount].nLength = nLength;
    g_arBufferDevice[g_nPendingBufferCount].nType = nType;
    g_arBufferDevice[g_nPendingBufferCount].nAddr = 0x0001; // address of device = chau nguyen

    for (BYTE nCount = 0; nCount < nLength; nCount++)
    {
        g_arBufferDevice[g_nPendingBufferCount].pData[nCount] = pBuffer[nCount];
    }
    g_nPendingBufferCount++;

    if (g_nPendingBufferCount == MAX_QUEUE_ZISE)
        ProcessBufferDevice(NULL);
}

INTERNAL VOID ReleaseBuffer()
{
    UARTBUFFER stBuffer = { 0 };
    for (BYTE nIndex = 0; nIndex < g_nPendingBufferCount; nIndex++)
    {
        if (g_arBufferDevice[nIndex].nLength != 0)
        {
            stBuffer.nLength = g_arBufferDevice[nIndex].nLength;
            stBuffer.nType = g_arBufferDevice[nIndex].nType;
            stBuffer.nAddr = g_arBufferDevice[nIndex].nAddr;
            stBuffer.pData = (PVOID) g_arBufferDevice[nIndex].pData;
            g_arBufferDevice[nIndex].nLength = 0;

            WriteUartIndirect(g_nUartPort, &stBuffer);
        }
    }
}

INTERNAL VOID ProcessBufferDevice(PVOID pData)
{
    if (g_nPendingBufferCount != 0)
    {
        ReleaseBuffer();
        g_nPendingBufferCount = 0;
    }
    StartShortTimer(UPDATE_BUFFER_INTERVAL, ProcessBufferDevice, NULL);
}


