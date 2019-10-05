#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "timer.h"
#include "stwd100.h"

#ifdef STM32F091
#define WD_GPIO_CLK 		(RCC->AHBENR)
#define WD_GPIO_CLK_EN		RCC_AHBENR_GPIOAEN
#define WD_GPIO_PORT		GPIOA
#define WD_GPIO_PIN			GPIO_Pin_3
#endif

#ifdef STM32F030
#define WD_GPIO_CLK 		(RCC->AHBENR)
#define WD_GPIO_CLK_EN		RCC_AHBENR_GPIOBEN
#define WD_GPIO_PORT		GPIOB
#define WD_GPIO_PIN			GPIO_Pin_0
#endif	

#define WD_TIMEOUT_MS		1200
#define WD_TOGGLE_PERIOD	300

INTERNAL VOID StwdToggle(PVOID pData)
{
	GPIO_WriteBit(WD_GPIO_PORT, WD_GPIO_PIN, (BitAction)(1 - GPIO_ReadInputDataBit(WD_GPIO_PORT, WD_GPIO_PIN)));
	StartShortTimer(WD_TOGGLE_PERIOD, StwdToggle, NULL);
}

VOID StwdStart()
{
	// enable GPIO clk
	WD_GPIO_CLK |= WD_GPIO_CLK_EN;
	GPIO_InitTypeDef gpioInitStruct;
	GPIO_StructInit(&gpioInitStruct);
 	gpioInitStruct.GPIO_Pin = WD_GPIO_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
    gpioInitStruct.GPIO_OType = GPIO_OType_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(WD_GPIO_PORT, &gpioInitStruct);
	StwdToggle(NULL);
}