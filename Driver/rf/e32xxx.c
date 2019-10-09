#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "e32xxx.h"

#define RF_GPIO_CLK 		(RCC->AHBENR)
#define RF_GPIO_CLK_EN		RCC_AHBENR_GPIOAEN
#define RF_GPIO_PORT		GPIOA
#define RF_M0_GPIO_PIN		GPIO_Pin_0
#define RF_M1_GPIO_PIN		GPIO_Pin_1

VOID RfInit()
{
	GPIO_InitTypeDef gpioInitStruct;
	RF_GPIO_CLK |= RF_GPIO_CLK_EN;
	GPIO_StructInit(&gpioInitStruct);
 	gpioInitStruct.GPIO_Pin = RF_M0_GPIO_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
    gpioInitStruct.GPIO_OType = GPIO_OType_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(RF_GPIO_PORT, &gpioInitStruct);
	gpioInitStruct.GPIO_Pin = RF_M1_GPIO_PIN;
    GPIO_Init(RF_GPIO_PORT, &gpioInitStruct);
	// using mode 0 - default address 0x0000  - default channel 0x17
	GPIO_WriteBit(RF_GPIO_PORT, RF_M0_GPIO_PIN, Bit_RESET);
	GPIO_WriteBit(RF_GPIO_PORT, RF_M1_GPIO_PIN, Bit_RESET);
}