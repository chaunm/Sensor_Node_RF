/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_i2c.h"
#include "i2c.h"
#include "sht3x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SHT3X_I2C							I2C1
#define SHT30_ADDRESS						0x44
#define GET_STATUS_COMMAND					0x
#define SHT3X_CMD_SINGLE_SHOT				{0x24, 0x00}
#define SHT3X_CMD_SINGLE_SHOT_STRETCH		{0x2C, 0x06}
#define SHT3X_CMD_PERIODIC_05_HIGH			{0x20, 0x32}
#define SHT3X_CMD_PERIDOIC_1_HIGH			{0x21, 0x30}
#define SHT3X_FETCH_DATA					{0xE0, oxoo}
/* more command can be send - refer to datasheet*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /* I2C Time out */
volatile uint32_t  SHT3xTimeout = I2C_TIMEOUT; 

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

/**
* @brief  Initializes the SHT3X by sending command to configure periodic mode.
* @param  None
* @retval None
*/
ErrorStatus SHT3x_Init(void)
{  
	BYTE dataNum;
	BYTE SHT3X_Cmd[2] = SHT3X_CMD_PERIDOIC_1_HIGH;
	
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
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(SHT3X_I2C, SHT30_ADDRESS, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	/* Reset local variable */
	dataNum = 0;
	
	/* Wait until all data are received */
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
		I2C_SendData(SHT3X_I2C, SHT3X_Cmd[dataNum]);
		/* Update number of received data */
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
	return SUCCESS;
}

uint16_t LM75_ReadTemp(void)
{   
  uint8_t LM75_BufferRX[2] ={0,0};
  uint16_t tmp = 0;
  uint32_t DataNum = 0;  
  
  /* Test on BUSY Flag */
  LM75Timeout = LM75_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(LM75_I2C, I2C_ISR_BUSY) != RESET)
  {
    if((LM75Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(LM75_I2C, LM75_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  /* Wait until TXIS flag is set */
  LM75Timeout = LM75_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(LM75_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((LM75Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
  }
  
  /* Send Register address */
  I2C_SendData(LM75_I2C, (uint8_t)LM75_REG_TEMP);
  
  /* Wait until TC flag is set */
  LM75Timeout = LM75_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(LM75_I2C, I2C_ISR_TC) == RESET)
  {
    if((LM75Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
  }  
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(LM75_I2C, LM75_ADDR, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  /* Reset local variable */
  DataNum = 0;
  
  /* Wait until all data are received */
  while (DataNum != 2)
  {
    /* Wait until RXNE flag is set */
    LM75Timeout = LM75_LONG_TIMEOUT;    
    while(I2C_GetFlagStatus(LM75_I2C, I2C_ISR_RXNE) == RESET)
    {
      if((LM75Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
    }
    
    /* Read data from RXDR */
    LM75_BufferRX[DataNum]= I2C_ReceiveData(LM75_I2C);
    
    /* Update number of received data */
    DataNum++;
  }    
  
  /* Wait until STOPF flag is set */
  LM75Timeout = LM75_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(LM75_I2C, I2C_ISR_STOPF) == RESET)
  {
    if((LM75Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
  }
  
  /* Clear STOPF flag */
  I2C_ClearFlag(LM75_I2C, I2C_ICR_STOPCF);
  
  /* Store LM75_I2C received data */
  tmp = (uint16_t)(LM75_BufferRX[0] << 8);
  tmp |= LM75_BufferRX[1];    
  
  /* Return Temperature value */
  return (uint16_t)(tmp >> 7);
}



