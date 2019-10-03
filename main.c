#include "flash.h"
#include "system.h"
#include "device.h"
#include "timer.h"
#include "parameter.h"
#include "led-segment.h"
#include "uart.h"
#include "device-desc.h"
#include "debug-led.h"

VOID InitRegister()
{
	TEMP_HIGH = 0;
	TEMP_HIGH = 0;
	TEMP_HIGH = 0;
	TEMP_HIGH = 0;
	ALARM = 0;
}

VOID OnDeviceRegisterProc(PVOID pEvent)
{
	PREGISTEREVENT pRegisterEvent = (PREGISTEREVENT)pEvent;
	switch (pRegisterEvent->nRegister)
	{
	default:
	  	break;
	}
}

VOID DefineParamProc(PVOID pParameter)
{
	DEFINE_PARAM(0, MIN_TEMP_PARAM, DEFAULT_MIN_TEMP);
	DEFINE_PARAM(1, MAX_TEMP_PARAM, DEFAULT_MAX_TEMP);
	DEFINE_PARAM(2, MIN_HUMI_PARAM, DEFAULT_MIN_HUMI);
	DEFINE_PARAM(3, MAX_HUMI_PARAM, DEFAULT_MAX_HUMI);
}


void main()
{
  	InitSystem();
	InitFlash();
	DebugLedStart();
  	OpenUartPort(UART_PORT_1, 9600);
	LedSegmentInit();
  	InitDevice(NULL);
	InitRegister();
	InitParameter(DefineParamProc);
	
	RegisterDeviceCallback(DEVICE_REGISTER_EVENT, OnDeviceRegisterProc);
//	RegisterParamCallback(PARAM_CHANGE_EVENT, ParamChangeProc);	
  	while(1)
  	{
	  	ProcessUartEvents();
		ProcessTimerEvents();
  	}
}
