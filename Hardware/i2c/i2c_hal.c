//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 44, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT3x Sample Code (V1.0)
// File      :  i2c_hal.c (V1.0)
// Author    :  RFU
// Date      :  16-Jun-2014
// Controller:  STM32F100RB
// IDE       :  µVision V4.71.2.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"

//-- Defines -------------------------------------------------------------------
// hardware definition 
#ifdef STM32F030
#define I2C_GPIO_CLK 				(RCC->AHBENR)
#define I2C_GPIO_CLK_ENABLE			RCC_AHBENR_GPIOBEN
#define I2C_GPIO_PORT				GPIOB
#define I2C_SCL_PIN					GPIO_Pin_6
#define I2C_SDA_PIN					GPIO_Pin_7
#define I2C_SCL_PIN_NUM				6
#define I2C_SDA_PIN_NUM				7
// I2C IO-Pins                         /* -- adapt the defines for your uC -- */
// SDA on port B, bit 7
#define SDA_LOW()  (GPIOB->BSRR = 0x00800000) // set SDA to low
#define SDA_OPEN() (GPIOB->BSRR = 0x00000080) // set SDA to open-drain
#define SDA_READ   (GPIOB->IDR  & 0x0080)     // read SDA

// SCL on port B, bit 6               /* -- adapt the defines for your uC -- */
#define SCL_LOW()  (GPIOB->BSRR = 0x00400000) // set SCL to low
#define SCL_OPEN() (GPIOB->BSRR = 0x00000040) // set SCL to open-drain
#define SCL_READ   (GPIOB->IDR  & 0x0040)     // read SCL
#endif

#ifdef STM32F091
#define I2C_GPIO_CLK 				(RCC->AHBENR)
#define I2C_GPIO_CLK_ENABLE			RCC_AHBENR_GPIOBEN
#define I2C_GPIO_PORT				GPIOB
#define I2C_SCL_PIN					GPIO_Pin_8
#define I2C_SDA_PIN					GPIO_Pin_9
#define I2C_SCL_PIN_NUM				8
#define I2C_SDA_PIN_NUM				9
// I2C IO-Pins                         /* -- adapt the defines for your uC -- */
// SDA on port B, bit 9
#define SDA_LOW()  (GPIOB->BSRR = 0x02000000) // set SDA to low
#define SDA_OPEN() (GPIOB->BSRR = 0x00000200) // set SDA to open-drain
#define SDA_READ   (GPIOB->IDR  & 0x0200)     // read SDA

// SCL on port B, bit 8               /* -- adapt the defines for your uC -- */
#define SCL_LOW()  (GPIOB->BSRR = 0x01000000) // set SCL to low
#define SCL_OPEN() (GPIOB->BSRR = 0x00000100) // set SCL to open-drain
#define SCL_READ   (GPIOB->IDR  & 0x0100)     // read SCL
#endif


void I2cDelayMicroSeconds(uint32_t nbrOfUs)
{
	uint32_t i;
	for(i = 0; i < nbrOfUs; i++)
	{  
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}

void I2c_Init(void)
{   
	GPIO_InitTypeDef gpioInitStruct;
	I2C_GPIO_CLK |= I2C_GPIO_CLK_ENABLE;  // I/O port B clock enabled
	
	SDA_OPEN();                  // I2C-bus idle mode SDA released  
	SCL_OPEN();                  // I2C-bus idle mode SCL released
	I2C_GPIO_CLK |= I2C_GPIO_CLK_ENABLE;
    gpioInitStruct.GPIO_Pin = I2C_SCL_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
    gpioInitStruct.GPIO_OType = GPIO_OType_OD;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_GPIO_PORT, &gpioInitStruct);
    gpioInitStruct.GPIO_Pin = I2C_SDA_PIN;
    GPIO_Init(I2C_GPIO_PORT, &gpioInitStruct);
}


void I2c_StartCondition(void)
{
	SDA_OPEN();
	I2cDelayMicroSeconds(1);
	SCL_OPEN();
	I2cDelayMicroSeconds(1);
	SDA_LOW();
	I2cDelayMicroSeconds(10);  // hold time start condition (t_HD;STA)
	SCL_LOW();
	I2cDelayMicroSeconds(10);
}


void I2c_StopCondition(void)
{
	SCL_LOW();
	I2cDelayMicroSeconds(1);
	SDA_LOW();
	I2cDelayMicroSeconds(1);
	SCL_OPEN();
	I2cDelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
	SDA_OPEN();
	I2cDelayMicroSeconds(10);
}


etError I2c_WriteByte(uint8_t txByte){
	etError error = NO_ERROR;
	uint8_t     mask;
	for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
	{
		if((mask & txByte) == 0) SDA_LOW(); // masking txByte, write bit to SDA-Line
		else                     SDA_OPEN();
		I2cDelayMicroSeconds(1);               // data set-up time (t_SU;DAT)
		SCL_OPEN();                         // generate clock pulse on SCL
		I2cDelayMicroSeconds(5);               // SCL high time (t_HIGH)
		SCL_LOW();
		I2cDelayMicroSeconds(1);               // data hold time(t_HD;DAT)
	}
	SDA_OPEN();                           // release SDA-line
	SCL_OPEN();                           // clk #9 for ack
	I2cDelayMicroSeconds(1);                 // data set-up time (t_SU;DAT)
	if(SDA_READ) error = ACK_ERROR;       // check ack from i2c slave
	SCL_LOW();
	I2cDelayMicroSeconds(20);                // wait to see byte package on scope
	return error;                         // return error code
}


etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout)
{
	etError error = NO_ERROR;
	uint8_t mask;
	*rxByte = 0x00;
	SDA_OPEN();                            // release SDA-line
	for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
	{ 
		SCL_OPEN();                          // start clock on SCL-line
		I2cDelayMicroSeconds(1);                // clock set-up time (t_SU;CLK)
		error = I2c_WaitWhileClockStreching(timeout);// wait while clock streching
		I2cDelayMicroSeconds(3);                // SCL high time (t_HIGH)
		if(SDA_READ) *rxByte |= mask;        // read bit
		SCL_LOW();
		I2cDelayMicroSeconds(1);                // data hold time(t_HD;DAT)
	}
	if(ack == ACK) SDA_LOW();              // send acknowledge if necessary
	else           SDA_OPEN();
	I2cDelayMicroSeconds(1);                  // data set-up time (t_SU;DAT)
	SCL_OPEN();                            // clk #9 for ack
	I2cDelayMicroSeconds(5);                  // SCL high time (t_HIGH)
	SCL_LOW();
	SDA_OPEN();                            // release SDA-line
	I2cDelayMicroSeconds(20);                 // wait to see byte package on scope
	
	return error;                          // return with no error
}

etError I2c_WaitWhileClockStreching(uint8_t timeout)
{
	etError error = NO_ERROR;
	
	while(SCL_READ == 0)
	{
		if(timeout-- == 0)	return TIMEOUT_ERROR;
		I2cDelayMicroSeconds(1000);
	}
	
	return error;
}


etError I2c_GeneralCallReset(void)
{
	etError error;
	
	I2c_StartCondition();
	error = I2c_WriteByte(0x00);
	if(error == NO_ERROR) error = I2c_WriteByte(0x06);
	
	return error;
}
