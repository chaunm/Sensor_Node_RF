/********************************************************************************
Bkav
Product: Platform STM32F100
Module: Header file of device.c module
Version: 1.0
Author: ThiVD
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
#ifndef __DEVICE_H__
#define __DEVICE_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
typedef struct tagREGISTEREVENT {
	BYTE nEvent;
	BYTE nRegister;
	BYTE nValue;
	BYTE nOldValue;
} REGISTEREVENT, *PREGISTEREVENT;

typedef struct tagPARAMEVENT {
	BYTE nEvent;
	WORD nParam;
	WORD nValue;
} PARAMEVENT, *PPARAMEVENT;
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define _PDEVICEREGISTER(buffer)        ((PDEVICEREGISTER)(buffer))
#define _PDEVICEPARAM(buffer)           ((PDEVICEPARAM)(buffer))

#define DEVICE_REGISTER_EVENT	        0
#define DEVICE_PARAM_EVENT		        1
#define DEVICE_EVENT_COUNT		        2
#define MAX_REGISTER_PART		        8
#define MAX_PARAM_PART			        4

#define ROOT                            0x0001
#define INVALID_REGISTER_VALUE 	        0xFF
#define CURRENT_REGISTER_VALUE	        0xFF
#define INVALID_PARAMETER_VALUE	        0xFFFF
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
EXTERNAL PBYTE REGISTERS;
/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID InitDevice(PVOID pDefinition);
VOID RegisterDeviceCallback(BYTE nEvent, SYSTEMCALLBACK fnCallback);
VOID ResetDevice();
BOOL IsFlashing();
VOID SetPriority(BYTE nPriority);
VOID SetDelay(WORD nDelay);
VOID ReportPortProc(PVOID pParameter);
VOID SetRegister(BYTE nRegister, BYTE nValue);
VOID RestoreRegister();
BOOL IsConnected();
BOOL GetTickFlag();
VOID ClearTickFlag();
WORD GetDeviceAddress();
/* 
To send command reset wifi module SendCommand(PAKAGE_TYPE_WIFI_COMMAND, RESET_WIFI_MODULE);
To send command reflash wifi module SendCommand(PAKAGE_TYPE_WIFI_COMMAND, REFLASH_WIFI_MODULE);
To send command get tick from Zigbee module SendCommand(PACKAGE_TYPE_GET_TICK, 0x00);
To send command reboot wireless module SendCommand(PACKAGE_TYPE_REBOOT_WIRELESS, 0x00);
*/
VOID SendCommand(BYTE nType, BYTE nCommand);
#endif //__DEVICE_H__
