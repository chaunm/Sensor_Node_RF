/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_i2c.h"
#include "i2c.h"
#include "sht3x.h"
#include "sht3x.h"
#include "timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SHT3X_I2C							I2C1
#define SHT30_ADDRESS						0x44

#define SHT3X_CMD_SINGLE_SHOT				{0x24, 0x00}
#define SHT3X_CMD_SINGLE_SHOT_STRETCH		{0x2C, 0x06}
#define SHT3X_CMD_PERIODIC_05_HIGH			{0x20, 0x32}
#define SHT3X_CMD_PERIDOIC_1_HIGH			{0x21, 0x30}
#define SHT3X_CMD_FETCH_DATA				{0xE0, 0x00}	
#define SHT3X_CMD_RESET						{0x30, 0xA2}
/* more command can be sent - refer to datasheet*/
#define POLYNOMIAL	0x31

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /* I2C Time out */
volatile uint32_t  SHT3xTimeout = I2C_TIMEOUT; 
static WORD rawTemp = 0;
static WORD rawHumi = 0;

static int8_t SHT3X_CRC(PBYTE idata, int len)
{
    /*
        Polynomial: 0x31 (x8 + x5 + x4 + 1)
        Initialization: 0xFF
        Final XOR: 0x00
        Example: CRC (0xBEEF) = 0x92
    */
    uint8_t crc = 0xFF;
    
    for ( int j = len; j; --j )
    {
        crc ^= *idata;
		idata++;
        for ( int i = 8; i; --i )
        {
            crc = ( crc & 0x80 )
            ? (crc << 1) ^ POLYNOMIAL
            : (crc << 1);
        }
    }
    return crc;
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
* @brief  Checks the SHT3x status.
* @param  None
* @retval ErrorStatus: SHT3x Status (ERROR or SUCCESS).
*/
ErrorStatus SHT3x_GetStatus(void)
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 0, I2C_AutoEnd_Mode, I2C_No_StartStop);
	
	/* Clear NACKF and STOPF */
	I2C_ClearFlag(SHT3X_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
	
	/* Generate start */
	I2C_GenerateSTART(SHT3X_I2C, ENABLE);  
	
	/* Wait until timeout elapsed */
	while ((I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_STOPF) == RESET) && (I2C_TimeOut-- != 0)); 
	
	/* Check if Temp sensor is ready for use */
	if ((I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_NACKF) != RESET) || (I2C_TimeOut == 0))
	{      
		/* Clear NACKF and STOPF */
		I2C_ClearFlag(SHT3X_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
		
		return ERROR;
	}
	else
	{
		/* Clear STOPF */
		I2C_ClearFlag(SHT3X_I2C, I2C_ICR_STOPCF);
		
		return SUCCESS;
	}    
}

INTERNAL VOID SHT3X_ReadData(PVOID pData)
{   
	BYTE readData[6] = {0};
	BYTE sth3xReadCmd[2] = SHT3X_CMD_FETCH_DATA;
	BYTE dataNum;
	
	/* Test on BUSY Flag */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return;
	}
		
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	/* Wait until TXIS flag is set */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
		
	dataNum = 2;
	while (dataNum != 2)
	{
		while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_TXIS) == RESET)
		{
			if((SHT3xTimeout--) == 0) 
				return;
		}
		/* Send Register address */
		I2C_SendData(SHT3X_I2C, sth3xReadCmd[dataNum]);
		/* update number of sent data */
		dataNum++;
	}		
	/* Wait until TC flag is set */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_TC) == RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return;
	}  
		
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 6, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
	/* Reset local variable */
	dataNum = 0;
		
	/* Wait until all data are received */
	while (dataNum != 6)
	{
		/* Wait until RXNE flag is set */
		SHT3xTimeout = SHT3X_LONG_TIMEOUT;    
		while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_RXNE) == RESET)
		{
			if((SHT3xTimeout--) == 0) 
				return;
		}
			
		/* Read data from RXDR */
		readData[dataNum]= I2C_ReceiveData(SHT3X_I2C);
		
		/* Update number of received data */
		dataNum++;
	}    
		
	/* Wait until STOPF flag is set */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_STOPF) == RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return;
	}
		
	/* Clear STOPF flag */
	I2C_ClearFlag(SHT3X_I2C, I2C_ICR_STOPCF);

	if (readData[2] == SHT3X_CRC(&(readData[0]), 2))
	{
		rawTemp = readData[1] + ((WORD)readData[0]) << 8;
	}
	if (readData[5] == SHT3X_CRC(&(readData[3]), 2))
	{
		rawHumi = readData[4] + ((WORD)readData[3]) << 8;
	}
	StartShortTimer(3000, SHT3X_ReadData, NULL);
}

/**
* @brief  Initializes the SHT3X by sending command to configure periodic mode.
* @param  None
* @retval None
*/
ErrorStatus SHT3X_Init()
{  
	BYTE dataNum;
	BYTE sht3xRstCmd[2] = SHT3X_CMD_RESET;
	BYTE sht3xConfCmd[2] = SHT3X_CMD_PERIDOIC_1_HIGH;
	
	if (!I2C_IsOpen())
		I2C_Open();
	// Send command to enable periodic mode
	/* Test on BUSY Flag */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return ERROR;
	}
	// Sending reset command
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	
	/* Reset local variable */
	dataNum = 0;
	
	/* Wait until all data are sent */
	while (dataNum != 2)
	{
		/* Wait until TXIS flag is set */
		SHT3xTimeout = SHT3X_LONG_TIMEOUT;
//		while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_TXIS) == RESET)    
		{
			while((SHT3xTimeout--) == 0) ;
//				return ERROR;
		}		
		/* Send command */
		I2C_SendData(SHT3X_I2C, sht3xRstCmd[dataNum]);
		/* Update number of sent data */
		dataNum++;
	}
	
	/* Wait until TC flag is set */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_STOPF) == RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return ERROR;
	}  
	I2C_ClearFlag(SHT3X_I2C, I2C_ISR_STOPF);
	
	/* wait for device reset */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while (SHT3xTimeout >0)
		SHT3xTimeout--;
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	/* Reset local variable */
	dataNum = 0;
	
	/* Wait until all data are sent */
	while (dataNum != 2)
	{
		/* Wait until TXIS flag is set */
		SHT3xTimeout = SHT3X_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_TXIS) == RESET)    
		{
			if((SHT3xTimeout--) == 0) 
				return ERROR;
		}		
		/* Send command */
		I2C_SendData(SHT3X_I2C, sht3xConfCmd[dataNum]);
		/* Update number of sent data */
		dataNum++;
	}
	
	/* Wait until TC flag is set */
	SHT3xTimeout = SHT3X_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SHT3X_I2C, I2C_ISR_TC) == RESET)
	{
		if((SHT3xTimeout--) == 0) 
			return ERROR;
	}  
	I2C_ClearFlag(SHT3X_I2C, I2C_ISR_TC);
	/* send configuration command */
	StartShortTimer(3000, SHT3X_ReadData, NULL);
	return SUCCESS;
}

ErrorStatus SHT3X_GetTemperature(int16_t* value)
{
	//  Conversion of raw values into a physical scale
    float temp = rawTemp;
    temp = (1750 * temp / 65535.0) - 450;
    *value = (int16_t)(temp);
	return SUCCESS;
}


ErrorStatus SHT3X_GetHumidity(uint16_t* value)
{
	//  Conversion of raw values into a physical scale
    float humi = rawHumi;
    humi = 1000 * (humi / 65535.0);
    *value = (int16_t)(humi);
	return SUCCESS;
}






