/********************************************************************************

Product: SHTMS

Module: uart.c

Version: 1.0

Author: NGUYEN KIM THANH

Created: 08-2014

Modified:
  <Chau Nguyen>
  <09-2019>
  <Change>

Released:

Description: Including functions for UART communication

Note: <Note>

********************************************************************************/

/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "stm32f0xx_misc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
#include "uart.h"
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define UART_BUFFER_SIZE	128 // chaunm check here
#define UART_PACKAGE_SIZE	128

// hardware definition 
#ifdef STM32F030
#define UART_GPIO_CLK 				(RCC->AHBENR)
#define UART_GPIO_CLK_ENABLE		RCC_AHBENR_GPIOAEN
#define UART_GPIO_PORT				GPIOA
#define UART_TX_PIN					GPIO_Pin_2
#define UART_RX_PIN					GPIO_Pin_3
#define UART_PIN_AF					GPIO_AF_1
#endif

#ifdef STM32F091
#define UART_GPIO_CLK 				(RCC->AHBENR)
#define UART_GPIO_CLK_ENABLE		RCC_AHBENR_GPIOAEN
#define UART_GPIO_PORT				GPIOA
#define UART_TX_PIN					GPIO_Pin_9
#define UART_RX_PIN					GPIO_Pin_10
#define UART_PIN_AF					GPIO_AF_1
#endif
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
BYTE g_nUartPort = 0;
INTERNAL UARTBUFFER g_stUartBuffer = { 0 };
INTERNAL SYSTEMCALLBACK fnUartCallBack[UART_EVENT_COUNT] = { NULL };

INTERNAL BYTE g_arReceiveBuffer[UART_BUFFER_SIZE] = { 0x00 };
INTERNAL BYTE g_nBufferIndex = 0;
INTERNAL BYTE g_nProcessIndex = 0;
INTERNAL BYTE g_arReceivePackage[UART_PACKAGE_SIZE] = { 0x00 };
INTERNAL BYTE g_nPackageIndex = 0;
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID USART1_IRQHandler(VOID);
VOID USART2_IRQHandler(VOID);
VOID USART3_4_IRQHandler(VOID);
/*-------------------------------------------------------------------------------
Function: VOID OpenUartPort(USART_TypeDef* USARTx, DWORD nBaudrate);

Purpose: Set baudrate speed for serial communication USARTx.

Parameters: USARTx, nBaudrate

Return: none

Comments: The baudrate speed depends on frequency of crystal.
      This project uses 24.00 MHz crystal

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
VOID OpenUartPort(BYTE nPort, DWORD nBaudrate)
{
	USART_TypeDef* USARTx;
    USART_InitTypeDef USART_InitStruct;
	GPIO_InitTypeDef gpioInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitStruct.USART_BaudRate = nBaudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No ;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 

    g_nUartPort = nPort;
	  switch (nPort)
	  {
	      case UART_PORT_1:
		      USARTx = USART1;
		      break;
	      default:
		      return;
	  }
	
    if (USARTx == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
	// configure GPIO
	UART_GPIO_CLK |= UART_GPIO_CLK_ENABLE;
    gpioInitStruct.GPIO_Pin = UART_TX_PIN;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF; 
    gpioInitStruct.GPIO_OType = GPIO_OType_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(UART_GPIO_PORT, &gpioInitStruct);
    gpioInitStruct.GPIO_Pin = UART_RX_PIN;
    GPIO_Init(UART_GPIO_PORT, &gpioInitStruct);
    GPIO_PinAFConfig(UART_GPIO_PORT, UART_TX_PIN, UART_PIN_AF);
    GPIO_PinAFConfig(UART_GPIO_PORT, UART_RX_PIN, UART_PIN_AF);
    USART_Init(USARTx, &USART_InitStruct);
	// enable RX interrupt
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    //USART_IT_TXEConfig(USARTx, ENABLE);
}
/*-------------------------------------------------------------------------------
Function: VOID OpenUartPortIndirect(PUARTCONFIG pParam);

Purpose: Enable open indirect USART port.

Parameters: pParam

Return: none

Comments: None

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
VOID OpenUartPortIndirect(PUARTCONFIG pParam)
{
	OpenUartPort(pParam->nPort, pParam->nBaudrate);
}

/*-------------------------------------------------------------------------------
Function: VOID USART_WriteByte(PUARTCONFIG pParam);

Purpose: Send one data byte over UART.

Parameters: pParam

Return: none

Comments: None

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
static VOID USART_WriteByte(USART_TypeDef* USARTx, BYTE data)
{
    USART_SendData(USARTx, (WORD)data);
}
/*-------------------------------------------------------------------------------
Function: VOID WriteUart(USART_TypeDef* USARTx, PVOID pData, BYTE nLen);

Purpose: Write nLen (byte) to USARTx port.

Parameters: USARTx, pData, nLen

Return: none

Comments: Using USART_WriteByte() function to send multi byte

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
VOID WriteUart(BYTE nPort, PVOID pData, BYTE nLen)
{
	USART_TypeDef* USARTx;
	PBYTE pBuffer = (PBYTE)pData;
	switch (nPort)
	{
	case UART_PORT_1:
		USARTx = USART1;
		break;
	default:
		return;
	}
    while (nLen)
    {
        USART_WriteByte(USARTx, *pBuffer++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        nLen--;
    }
}
/*-------------------------------------------------------------------------------
Function: WriteUartIndirect(PUARTBUFFER pUartBuffer)

Purpose: Write nLen (byte) to USARTx port.

Parameters: USARTx, pData, nLen

Return: none

Comments: Using USART_WriteByte() function to send multi byte

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
VOID WriteUartIndirect(BYTE nPort, PUARTBUFFER pUartBuffer)
{
    USART_TypeDef* USARTx;
    PBYTE pBuffer = pUartBuffer->pData;

    switch (nPort)
	{
	case UART_PORT_1:
		USARTx = USART1;
		break;
	default:
		return;
	}
	// chau nguyen - need to add header for RF here
	/* header for RF Module - broadcast type 0xFFFF + channel*/
	USART_WriteByte(USARTx, 0xFF);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	
	USART_WriteByte(USARTx, 0xFF);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	
	USART_WriteByte(USARTx, 0x03); // RF channel
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	
    USART_WriteByte(USARTx, PACKAGE_START_BYTE);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);

    USART_WriteByte(USARTx, pUartBuffer->nLength + PACKAGE_EXPAND_LENGTH);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);

    USART_WriteByte(USARTx, pUartBuffer->nType);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);

    USART_WriteByte(USARTx, pUartBuffer->nAddr);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);

    USART_WriteByte(USARTx, pUartBuffer->nAddr >> 8);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);

    for (BYTE nIndex = 0; nIndex < pUartBuffer->nLength; nIndex++)
    {
        USART_WriteByte(USARTx, pBuffer[nIndex]);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }

    USART_WriteByte(USARTx, PACKAGE_STOP_BYTE);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}
/*-------------------------------------------------------------------------------
Function: VOID PlatformUartInterrupt(VOID);

Purpose: Saving data receiver from USART2 port to g_arReceiveBuffer[].

Parameters: None

Return: none

Comments: Max buffer size of g_arReceiveBuffer[] is 255.

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
//VOID PlatformUartInterrupt(VOID)
//{
//    if (USART_GetITFlagRXNE(USART2) != RESET)
//    {
//            g_arReceiveBuffer[g_nBufferIndex] = USART_ReceiveByte(USART2);
//            g_nBufferIndex++;
//            if (g_nBufferIndex >= UART_BUFFER_SIZE)
//			{
//                g_nBufferIndex = 0;
//			}
//    }
//}
/*-------------------------------------------------------------------------------
Function: VOID RegisterUartCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback, PVOID pData);

Purpose: Registering a callback function respectively an event.

Parameters: nEvent, fnCallback, pData

Return: None

Comments: Max Event is 1.

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
VOID RegisterUartCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback)
{
	if (nEvent < UART_EVENT_COUNT)
	{
		fnUartCallBack[nEvent] = fnCallback;
	}
}
/*-------------------------------------------------------------------------------
Function: BOOL ProcessUartEvents(VOID) ;

Purpose: Processing newsletter at g_arReceiveBuffer[] and excuting function respectively.

Parameters: None

Return: TRUE

Comments: Data format:
			START 	LENGTH 	TYPE 	  ID      	DATA      STOP
			 AA	 	  07   	 01  	01 02   01 02 03 04   55

Modified:
  <Modified by>
  <Date>
  <Change>
--------------------------------------------------------------------------------*/
BOOL ProcessUartEvents(VOID)
{
	while (g_nProcessIndex != g_nBufferIndex)	// new item received
	{
		g_arReceivePackage[g_nPackageIndex] = g_arReceiveBuffer[g_nProcessIndex];
		if (g_nPackageIndex == g_arReceivePackage[1] + 2)
		{
			if (g_arReceiveBuffer[g_nProcessIndex] == PACKAGE_STOP_BYTE)
			{
                g_stUartBuffer.nLength = g_arReceivePackage[1] - 3;
                g_stUartBuffer.nType = g_arReceivePackage[2];
				g_stUartBuffer.nAddr = g_arReceivePackage[3] + (((WORD)g_arReceivePackage[4]) << 8);
                g_stUartBuffer.pData = &g_arReceivePackage[5];
                if (fnUartCallBack[UART_DATA_EVENT] != NULL)
				{
					fnUartCallBack[UART_DATA_EVENT](&g_stUartBuffer);
				}
			}
			g_nPackageIndex = 0;	// reset index
		}
		else if (((g_nPackageIndex == 0) && (g_arReceiveBuffer[g_nProcessIndex] == PACKAGE_START_BYTE))\
            || (g_nPackageIndex > 0))
		{
			g_nPackageIndex++;
			if (g_nPackageIndex >= UART_PACKAGE_SIZE)
			{
				g_nPackageIndex = 0;
			}
		}

		g_nProcessIndex++;
		if (g_nProcessIndex >= UART_BUFFER_SIZE)
		{
			g_nProcessIndex = 0;
		}
	}
	return TRUE;
}
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/
VOID USART1_IRQHandler(VOID)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		g_arReceiveBuffer[g_nBufferIndex] = (BYTE)USART_ReceiveData(USART1);
		g_nBufferIndex = (g_nBufferIndex + 1) % UART_BUFFER_SIZE;
	}
}
