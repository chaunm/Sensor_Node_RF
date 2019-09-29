#include "stm32f0xx_gpio.h"
#include "led-segment.h"
#include "system.h"

#define NUMBER_OF_LED   12
#define LED_GPIO_PORT	GPIOA
#define LED_GPIO_CLK	(RCC->AHBENR)
#define SHCP 			0
#define STCP 			1
#define DS   			2

#define TRIGGER_SHCP() { GPIO_ResetBits(LED_GPIO_PORT, SHCP); GPIO_SetBits(LED_GPIO_PORT, SHCP); }

#define TRIGGER_STCP() { GPIO_ResetBits(LED_GPIO_PORT, STCP); GPIO_SetBits(LED_GPIO_PORT, STCP); }
	
BYTE ledData[NUMBER_OF_LED] = {0};

VOID OutputToPortQ();
VOID InitZeroDetect();

static VOID ledSegmentOutput()
{
	BYTE nDigit, byIndex;
    for (nDigit = 0; nDigit < NUMBER_OF_LED; nDigit++)
	{
	  	for (byIndex = 8; byIndex > 0; byIndex --)
	  	{
			GPIO_WriteBit(LED_GPIO_PORT, DS, (BitAction)CHECK_BIT(ledData[nDigit], (byIndex -1)));
			TRIGGER_SHCP();
	  	}
	}
    TRIGGER_STCP();
}

VOID LedSegmentInit()
{
  	GPIO_InitTypeDef gpioInitStruct;
	// chaunm - need to initialize IO for controlling 74HC595
	LED_GPIO_CLK |= RCC_AHBENR_GPIOAEN; // enable GPIO clk
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	gpioInitStruct.GPIO_Pin = SHCP;
	GPIO_Init(LED_GPIO_PORT, &gpioInitStruct);
	gpioInitStruct.GPIO_Pin = STCP;
	GPIO_Init(LED_GPIO_PORT, &gpioInitStruct);
	gpioInitStruct.GPIO_Pin = DS;
	GPIO_Init(LED_GPIO_PORT, &gpioInitStruct);
	GPIO_SetBits(LED_GPIO_PORT, SHCP);
	GPIO_SetBits(LED_GPIO_PORT, STCP);
	GPIO_ResetBits(LED_GPIO_PORT, DS);
	ledSegmentOutput();
}

VOID LedSegmentUpdateData(PBYTE pData, BYTE nLength)
{
    BYTE nDataCount;
	BYTE i;
	nDataCount = (nLength < NUMBER_OF_LED) ? nLength : NUMBER_OF_LED;
	CopyMemory(ledData, pData, nDataCount);
	for (i = nDataCount; i < NUMBER_OF_LED; i++)
	  	ledData[i] = 0;
	ledSegmentOutput();
}