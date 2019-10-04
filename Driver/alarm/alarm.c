#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "timer.h"
#include "alarm.h"

#ifdef STM32F091
#define ALARM_GPIO_CLK 		(RCC->AHBENR)
#define ALARM_GPIO_CLK_EN	RCC_AHBENR_GPIOBEN
#define ALARM_GPIO_PORT		GPIOB
#define ALARM_GPIO_PIN		GPIO_Pin_3
#endif

#ifdef STM32F030
#define ALARM_GPIO_CLK 		(RCC->AHBENR)
#define ALARM_GPIO_CLK_EN	RCC_AHBENR_GPIOBEN
#define ALARM_GPIO_PORT		GPIOB
#define ALARM_GPIO_PIN		GPIO_Pin_3
#endif

VOID AlarmInit()
{
	// enable GPIO clk
	ALARM_GPIO_CLK |= ALARM_GPIO_CLK_EN;
	GPIO_InitTypeDef gpioInitStruct;
	GPIO_StructInit(&gpioInitStruct);
 	gpioInitStruct.GPIO_Pin = ALARM_GPIO_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
    gpioInitStruct.GPIO_OType = GPIO_OType_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(ALARM_GPIO_PORT, &gpioInitStruct);
	GPIO_WriteBit(ALARM_GPIO_PORT, ALARM_GPIO_PIN, Bit_RESET);
}

VOID AlarmOn(PVOID pData)
{
	GPIO_WriteBit(ALARM_GPIO_PORT, ALARM_GPIO_PIN, Bit_SET);
}

VOID AlarmOff(PVOID pData)
{
	GPIO_WriteBit(ALARM_GPIO_PORT, ALARM_GPIO_PIN, Bit_RESET);
}

