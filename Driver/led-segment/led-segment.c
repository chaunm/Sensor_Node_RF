#include "stm32f0xx_gpio.h"
#include "led-segment.h"
#include "system.h"

#define NUMBER_OF_LED   6
#ifdef STM32F091
#define LED_GPIO_PORT	GPIOA
#define LED_GPIO_CLK	(RCC->AHBENR)
#define SHCP 			GPIO_Pin_6
#define STCP 			GPIO_Pin_4
#define DS   			GPIO_Pin_7
#endif

#ifdef STM32F030
#define LED_GPIO_PORT	GPIOA
#define LED_GPIO_CLK	(RCC->AHBENR)
#define SHCP 			GPIO_Pin_6
#define STCP 			GPIO_Pin_5
#define DS   			GPIO_Pin_7
#endif

#define TRIGGER_SHCP() { GPIO_ResetBits(LED_GPIO_PORT, SHCP); LedSegmentDelay(); GPIO_SetBits(LED_GPIO_PORT, SHCP); }
#define TRIGGER_STCP() { GPIO_ResetBits(LED_GPIO_PORT, STCP); LedSegmentDelay(); GPIO_SetBits(LED_GPIO_PORT, STCP); }

INTERNAL BYTE font[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
INTERNAL BYTE ledData[NUMBER_OF_LED];

VOID OutputToPortQ();
VOID InitZeroDetect();

#pragma optimize=none
VOID LedSegmentDelay()
{
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

INTERNAL VOID LedSegmentOutput()
{
	BYTE nDigit, byIndex;
    for (nDigit = 0; nDigit < NUMBER_OF_LED; nDigit++)
	{
	  	for (byIndex = 8; byIndex > 0; byIndex --)
	  	{
			GPIO_WriteBit(LED_GPIO_PORT, DS, (BitAction)CHECK_BIT(ledData[nDigit], (byIndex - 1)));
			TRIGGER_SHCP();
	  	}
	}
    TRIGGER_STCP();
}


VOID LedSegmentInit()
{
  	GPIO_InitTypeDef gpioInitStruct;
	BYTE i;
	// init display data
	for (i = 0; i < NUMBER_OF_LED; i++)
		ledData[i] = font[0];
	ledData[1] |= 0x80;
	ledData[4] |= 0x80;
	//  need to initialize IO for controlling 74HC595
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
	LedSegmentOutput();
}

VOID LedSegmentUpdateData(PBYTE pData, BYTE nLength)
{
    BYTE nDataCount;
	BYTE i;
	nDataCount = (nLength < NUMBER_OF_LED) ? nLength : NUMBER_OF_LED;
	CopyMemory(ledData, pData, nDataCount);
	for (i = nDataCount; i < NUMBER_OF_LED; i++)
	  	ledData[i] = 0;
	LedSegmentOutput();
}

// display
VOID LedSegmentDisplayMeasureValue(int16_t temp, uint16_t humi)
{
	if (temp < 100)
	{
		ledData[0] = font[temp % 10];
		ledData[1] = font[(temp / 10) % 10] | 0x80; // add DP
		ledData[2] = font[temp / 100];
	}
	else
	{
		ledData[0] = font[0];
		ledData[1] = font[0];
		ledData[2] = font[1];
	}
	if (humi < 100)
	{
		ledData[3] = font[humi % 10];
		ledData[4] = font[(humi / 10) % 10] | 0x80;
		ledData[5] = font[(humi / 100)];
	}
	else
	{
		ledData[0] = font[0];
		ledData[1] = font[0];
		ledData[2] = font[1];
	}
	LedSegmentOutput();
}
