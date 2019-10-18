#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32F0xx_i2c.h"
#include "i2c.h"

// hardware definition 
#ifdef STM32F030
#define I2C_GPIO_CLK 				(RCC->AHBENR)
#define I2C_GPIO_CLK_ENABLE			RCC_AHBENR_GPIOBEN
#define I2C_GPIO_PORT				GPIOB
#define I2C_SCL_PIN					GPIO_Pin_6
#define I2C_SDA_PIN					GPIO_Pin_7
#define I2C_SCL_PIN_NUM				6
#define I2C_SDA_PIN_NUM				7
#define I2C_PIN_AF					GPIO_AF_1
#endif

#ifdef STM32F091
#define I2C_GPIO_CLK 				(RCC->AHBENR)
#define I2C_GPIO_CLK_ENABLE			RCC_AHBENR_GPIOBEN
#define I2C_GPIO_PORT				GPIOB
#define I2C_SCL_PIN					GPIO_Pin_8
#define I2C_SDA_PIN					GPIO_Pin_9
#define I2C_SCL_PIN_NUM				8
#define I2C_SDA_PIN_NUM				9
#define I2C_PIN_AF					GPIO_AF_1
#endif

#ifdef SENSOR_HTS221
#define TIMING_8MHZ					0x00901D36
#define TIMING_48MHZ				0x30E32E37
#endif

#ifdef SENSOR_SHT30
#define TIMING_48MHZ				0x60100839
#endif
static BOOL isI2COpen = FALSE;

VOID I2C_Open()
{
	GPIO_InitTypeDef gpioInitStruct;
	I2C_InitTypeDef i2cInitStruct;
	// configure GPIO
	I2C_GPIO_CLK |= I2C_GPIO_CLK_ENABLE;
    gpioInitStruct.GPIO_Pin = I2C_SCL_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF; 
    gpioInitStruct.GPIO_OType = GPIO_OType_OD;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_GPIO_PORT, &gpioInitStruct);
    gpioInitStruct.GPIO_Pin = I2C_SDA_PIN;
    GPIO_Init(I2C_GPIO_PORT, &gpioInitStruct);
    GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SCL_PIN_NUM, I2C_PIN_AF);
    GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SDA_PIN_NUM, I2C_PIN_AF);
	// configure i2c - standard mode 100Hz
	// enable I2C clock
	RCC->CFGR3 |= RCC_CFGR3_I2C1SW;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	i2cInitStruct.I2C_Timing = TIMING_48MHZ;
	i2cInitStruct.I2C_Mode = I2C_Mode_SMBusHost;
	i2cInitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	i2cInitStruct.I2C_DigitalFilter = 0x00;
	i2cInitStruct.I2C_OwnAddress1 = 0x00;
	i2cInitStruct.I2C_Ack = I2C_Ack_Enable;
	i2cInitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2cInitStruct);
	I2C_Cmd(I2C1, ENABLE);	
	isI2COpen = TRUE;
}

BOOL I2C_IsOpen()
{
	return isI2COpen;
}