#include "stm32f0xx_i2c.h"
#include "i2c.h"
#include "hts221.h"

#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /* I2C Time out */

DWORD nHts221Timeout;

ErrorStatus HTS221_GetStatus(void)
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, 0, I2C_AutoEnd_Mode, I2C_No_StartStop);
	
	/* Clear NACKF and STOPF */
	I2C_ClearFlag(HTS221_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
	
	/* Generate start */
	I2C_GenerateSTART(HTS221_I2C, ENABLE);  
	
	/* Wait until timeout elapsed */
	while ((I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_STOPF) == RESET) && (I2C_TimeOut-- != 0)); 
	
	/* Check if Temp sensor is ready for use */
	if ((I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_NACKF) != RESET) || (I2C_TimeOut == 0))
	{      
		/* Clear NACKF and STOPF */
		I2C_ClearFlag(HTS221_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
		
		return ERROR;
	}
	else
	{
		/* Clear STOPF */
		I2C_ClearFlag(HTS221_I2C, I2C_ICR_STOPCF);
		
		return SUCCESS;
	}    
}

BYTE HTS221_ReadOneReg(BYTE RegName)
{   
	BYTE HTS221_BufferRX[1] ={0};
	WORD tmp = 0;   
	DWORD dataNum = 0;
	
	/* Test on BUSY Flag */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((nHts221Timeout--) == 0) 
			return 0xFF;
	}
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	/* Wait until TXIS flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return 0xFF;
	}
	
	/* Send Register address */
	I2C_SendData(HTS221_I2C, (BYTE)RegName);
	
	/* Wait until TC flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TC) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return 0xFF;
	}  
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
	/* Reset local variable */
	dataNum = 0;
	
	/* Wait until all data are received */
	while (dataNum != 1)
	{
		/* Wait until RXNE flag is set */
		nHts221Timeout = HTS221_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_RXNE) == RESET)    
		{
			if((nHts221Timeout--) == 0) 
				return 0xFF;
		}
		
		/* Read data from RXDR */
		HTS221_BufferRX[dataNum]= I2C_ReceiveData(HTS221_I2C);
		
		/* Update number of received data */
		dataNum++;
	}    
	
	/* Wait until STOPF flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_STOPF) == RESET)   
	{
		if((nHts221Timeout--) == 0) 
			return 0xFF;
	}
	
	/* Clear STOPF flag */
	I2C_ClearFlag(HTS221_I2C, I2C_ICR_STOPCF);
	
	/* Store HTS221_I2C received data */
	tmp = HTS221_BufferRX[0];
	
	
	/* return a Reg value */
	return tmp;  
}

ErrorStatus HTS221_ReadRegs(BYTE RegName, BYTE nDataCount, PBYTE pData)
{  
	
	DWORD dataNum = 0;
	
	/* Test on BUSY Flag */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	/* Wait until TXIS flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Send Register address */
	if (nDataCount > 1)
		I2C_SendData(HTS221_I2C, (BYTE)(RegName | 0x80));
	else
		I2C_SendData(HTS221_I2C, (BYTE)RegName);
	
	/* Wait until TC flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TC) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}  
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, nDataCount, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
	/* Reset local variable */
	dataNum = 0;
	
	/* Wait until all data are received */
	while (dataNum != nDataCount)
	{
		/* Wait until RXNE flag is set */
		nHts221Timeout = HTS221_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_RXNE) == RESET)    
		{
			if((nHts221Timeout--) == 0) 
				return ERROR;
		}
		
		/* Read data from RXDR */
		pData[dataNum]= I2C_ReceiveData(HTS221_I2C);
		
		/* Update number of received data */
		dataNum++;
	}    
	
	/* Wait until STOPF flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_STOPF) == RESET)   
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Clear STOPF flag */
	I2C_ClearFlag(HTS221_I2C, I2C_ICR_STOPCF);
	return SUCCESS;
}

ErrorStatus HTS221_WriteOneReg(BYTE RegName, BYTE data)
{   
	
	/* Test on BUSY Flag */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	
	/* Wait until TXIS flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Send Register address */
	I2C_SendData(HTS221_I2C, (BYTE)RegName);
	
	/* Wait until TC flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	I2C_SendData(HTS221_I2C, (BYTE)data);
		
	/* Wait until STOPF flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_STOPF) == RESET)   
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Clear STOPF flag */
	I2C_ClearFlag(HTS221_I2C, I2C_ICR_STOPCF);
	return SUCCESS;
	
}

ErrorStatus HTS221_WriteRegs(BYTE RegName, BYTE nCount, PBYTE data)
{   
	if (nCount == 0)
		return SUCCESS;	
	/* Test on BUSY Flag */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_BUSY) != RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(HTS221_I2C, HTS221_ADDR, nCount + 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	
	/* Wait until TXIS flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Send Register address */
	if (nCount > 1)
		I2C_SendData(HTS221_I2C, (BYTE)(RegName | 0x80));
	else
		I2C_SendData(HTS221_I2C, (BYTE)RegName);
	/* Wait until TC flag is set */
	while (nCount > 0)
	{
		nHts221Timeout = HTS221_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_TXIS) == RESET)
		{
			if((nHts221Timeout--) == 0) 
				return ERROR;
		}
		I2C_SendData(HTS221_I2C, *data);
		nCount--;
		data++;
	}
		
	/* Wait until STOPF flag is set */
	nHts221Timeout = HTS221_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(HTS221_I2C, I2C_ISR_STOPF) == RESET)   
	{
		if((nHts221Timeout--) == 0) 
			return ERROR;
	}
	
	/* Clear STOPF flag */
	I2C_ClearFlag(HTS221_I2C, I2C_ICR_STOPCF);
	return SUCCESS;
	
}

ErrorStatus HTS221_Init()
{
	ErrorStatus i2cStatus;
	BYTE inData, outData;
	DWORD configData = 0x00000081;
	DWORD configDataRead = 0x00FFFFFF;
	BYTE hts221Name;
	if (!I2C_IsOpen())
		I2C_Open();
	i2cStatus = HTS221_GetStatus();
	hts221Name = HTS221_ReadOneReg(HTS221_WHO_AM_I);
	if (hts221Name != HTS221_NAME)
		return ERROR;
	inData = 0x24;
	i2cStatus = HTS221_WriteRegs(HTS221_AV_CONF, 1, &inData);
	outData = HTS221_ReadOneReg(HTS221_AV_CONF);
	if ((inData != outData) || (i2cStatus == ERROR))
		return ERROR;
	i2cStatus = HTS221_WriteRegs(HTS221_CTRL_REG1, 3, (PBYTE)&configData);
	outData = HTS221_ReadRegs(HTS221_CTRL_REG1, 3, (PBYTE)&configDataRead);
	if ((configData != configDataRead) || (i2cStatus == ERROR))
		return ERROR;
	return SUCCESS;
}

ErrorStatus HTS221_GetTemperature(int16_t* value)
{
	int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
	int16_t T0_degC, T1_degC;
	uint8_t buffer[4], tmp;
	int32_t tmp32;
	/*1. Read from 0x32 & 0x33 registers the value of coefficients T0_degC_x8 and T1_degC_x8*/
	if(HTS221_ReadRegs(HTS221_T0_DEGC_X8, 2, buffer) == ERROR)
		return ERROR;
	/*2. Read from 0x35 register the value of the MSB bits of T1_degC and T0_degC */
	if(HTS221_ReadRegs(HTS221_T1_T0_MSB, 1, &tmp) == ERROR)
		return ERROR;
	/*Calculate the T0_degC and T1_degC values*/
	T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer[0]);
	T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer[1]);
	T0_degC = T0_degC_x8_u16>>3;
	T1_degC = T1_degC_x8_u16>>3;
	/*3. Read from 0x3C & 0x3D registers the value of T0_OUT*/
	/*4. Read from 0x3E & 0x3F registers the value of T1_OUT*/
	if(HTS221_ReadRegs(HTS221_T0_OUT_L, 4, buffer) == ERROR)
		return ERROR;
	T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	T1_out = (((uint16_t)buffer[3])<<8) | (uint16_t)buffer[2];
	/* 5.Read from 0x2A & 0x2B registers the value T_OUT (ADC_OUT).*/
	if(HTS221_ReadRegs(HTS221_TEMP_OUT_L, 2, buffer) == ERROR)
		return ERROR;
	T_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	/* 6. Compute the Temperature value by linear interpolation*/
	tmp32 = ((int32_t)(T_out - T0_out)) * ((int32_t)(T1_degC - T0_degC)*10);
	*value = tmp32 /(T1_out - T0_out) + T0_degC*10;
	return SUCCESS;
}

ErrorStatus HTS221_GetHumidity(uint16_t* value)
{
	int16_t H0_T0_out, H1_T0_out, H_T_out;
	int16_t H0_rh, H1_rh;
	uint8_t buffer[2];
	int32_t tmp;
	/* 1. Read H0_rH and H1_rH coefficients*/
	if(HTS221_ReadRegs(HTS221_H0_RH_X2, 2, buffer) == ERROR)	
		return ERROR;
	H0_rh = buffer[0]>>1;
	H1_rh = buffer[1]>>1;
	/*2. Read H0_T0_OUT */
	if(HTS221_ReadRegs(HTS221_H0_T0_OUT_L, 2, buffer) == ERROR)
		return ERROR;
	H0_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	/*3. Read H1_T0_OUT */
	if(HTS221_ReadRegs(HTS221_H1_T0_OUT_L, 2, buffer) == ERROR)
		return ERROR;
	H1_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	/*4. Read H_T_OUT */
	if(HTS221_ReadRegs(HTS221_HR_OUT_L, 2, buffer) == ERROR)
		return ERROR;
	H_T_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];
	/*5. Compute the RH [%] value by linear interpolation */
	tmp = ((int32_t)(H_T_out - H0_T0_out)) * ((int32_t)(H1_rh - H0_rh)*10);
	*value = (tmp/(H1_T0_out - H0_T0_out) + H0_rh*10);
	/* Saturation condition*/
	if(*value>1000) *value = 1000;
	return SUCCESS;
}
