/********************************************************************************
Bkav
Product: 
Module: 
Version: 1.0
Author: DongNX
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
#ifndef __UART_H__
#define __UART_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
/*
#pragma pack(1)
typedef struct tagUARTDATAEVENT
{
BYTE nSize;
BYTE nEvent;
PBYTE pData;
} UARTDATAEVENT, *PUARTDATAEVENT;

#pragma pack(1)
typedef struct tagUARTBUFFERSEND
{
BYTE nSize;
BYTE nEvent;
BYTE pData[MAX_UART_DATA];
} UARTBUFFERSEND, *PUARTBUFFERSEND;
*/
typedef struct tagUARTBUFFER
{
    BYTE nLength;
    BYTE nType;
    WORD nAddr;
    PBYTE pData;
} UARTBUFFER, *PUARTBUFFER;

typedef struct tagUARTCONFIG
{
    BYTE nPort;
    BYTE nBaudrate;
} UARTCONFIG, *PUARTCONFIG;
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
// Port Definition
#define UART_PORT_0             0x00
#define UART_PORT_1             0x01
#define UART_PORT_2             0x02 //
// Uart Event 
#define UART_DATA_EVENT			0

#define UART_EVENT_COUNT		1  

#define PACKAGE_START_BYTE		0xAA
#define PACKAGE_STOP_BYTE		0x55

#define PACKAGE_EXPAND_LENGTH   3 //1 byte Type and 2 byte nAddr

//#define MAX_UART_DATA           16
//#define UART_BUFFER_SEND        10
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
EXTERNAL BYTE g_nUartPort;
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID OpenUartPort(BYTE nPort, DWORD nBaudrate);
VOID OpenUartPortIndirect(PUARTCONFIG pParam);
VOID WriteUart(BYTE nPort, PVOID pData, BYTE nLen);
VOID WriteUartIndirect(BYTE nPort, PUARTBUFFER pUartBuffer);
VOID RegisterUartCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback);
BOOL ProcessUartEvents(VOID);

#endif // __USART_H__

