#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "timer.h"
#include "debug-led.h"

#ifdef STM32F091
#define DEBUG_LED_GPIO_CLK 		(RCC->AHBENR)
#define DEBUG_LED_GPIO_CLK_EN	RCC_AHBENR_GPIOAEN
#define DEBUG_LED_GPIO_PORT		GPIOA
#define DEBUG_LED_GPIO_PIN		GPIO_Pin_5
#endif

#ifdef STM32F030
#define DEBUG_LED_GPIO_CLK 		(RCC->AHBENR)
#define DEBUG_LED_GPIO_CLK_EN	RCC_AHBENR_GPIOBEN
#define DEBUG_LED_GPIO_PORT		GPIOB
#define DEBUG_LED_GPIO_PIN		GPIO_Pin_1
#endif

INTERNAL VOID DebugLedBlink(PVOID pData)
{
	GPIO_WriteBit(DEBUG_LED_GPIO_PORT, DEBUG_LED_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(DEBUG_LED_GPIO_PORT, DEBUG_LED_GPIO_PIN)));
	StartShortTimer(1000, DebugLedBlink, NULL);
}

VOID DebugLedStart()
{
	// enable GPIO clk
	DEBUG_LED_GPIO_CLK |= DEBUG_LED_GPIO_CLK_EN;
	GPIO_InitTypeDef gpioInitStruct;
	GPIO_StructInit(&gpioInitStruct);
 	gpioInitStruct.GPIO_Pin = DEBUG_LED_GPIO_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
    gpioInitStruct.GPIO_OType = GPIO_OType_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(DEBUG_LED_GPIO_PORT, &gpioInitStruct);
	DebugLedBlink(NULL);
}
