#ifndef __HTS221_H__
#define __HTS221_H__

#define HTS221_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define HTS221_LONG_TIMEOUT         ((uint32_t)(10 * HTS221_FLAG_TIMEOUT)) 

#define HTS221_I2C		  I2C1
#define HTS221_ADDR       0xBE
#define HTS221_NAME		  0xBC

#define HTS221_WHO_AM_I          	0x0F
#define HTS221_AV_CONF           	0x10
#define HTS221_CTRL_REG1         	0x20
#define HTS221_CTRL_REG2         	0x21
#define HTS221_CTRL_REG3         	0x22
#define HTS221_STATUS_REG        	0x27
#define HTS221_HR_OUT_L       		0x28
#define HTS221_HR_OUT_H       		0x29
#define HTS221_TEMP_OUT_L        	0x2A
#define HTS221_TEMP_OUT_H        	0x2B
#define HTS221_H0_RH_X2				0x30
#define HTS221_H1_RH_X2				0x31
#define HTS221_T0_DEGC_X8			0x32
#define HTS221_T1_DEGC_X8			0x33
#define HTS221_T1_T0_MSB			0x35
#define HTS221_H0_T0_OUT_L			0x36
#define HTS221_H0_T0_OUT_H			0x37
#define HTS221_H1_T0_OUT_L			0x3A
#define HTS221_H1_T0_OUT_H			0x3B
#define HTS221_T0_OUT_L				0x3C
#define HTS221_T0_OUT_H				0x3D
#define HTS221_T1_OUT_L				0x3E
#define HTS221_T1_OUT_H				0x3F

BYTE HTS221_ReadOneReg(BYTE RegName);
ErrorStatus HTS221_ReadRegs(BYTE RegName, BYTE nDataCount, PBYTE pData);
ErrorStatus HTS221_WriteOneReg(BYTE RegName, BYTE data);
ErrorStatus HTS221_WriteRegs(BYTE RegName, BYTE nCount, PBYTE data);
ErrorStatus HTS221_GetStatus(void);
ErrorStatus HTS221_Init();
ErrorStatus HTS221_GetTemperature(int16_t* value);
ErrorStatus HTS221_GetHumidity(uint16_t* value);

#endif // __HTS221_H__