#include "stm32f0xx.h"
#include "flash.h"
#include "system.h"
#include "device.h"
#include "timer.h"
#include "parameter.h"
#include "led-segment.h"
#include "uart.h"
#include "i2c.h"
#include "device-desc.h"
#include "debug-led.h"
#include "alarm.h"
#include "stwd100.h"
#include "sensor.h"
#include "measure.h"
#include "e32xxx.h"
#include "device-registers.h"
#include "device-parameter.h"

ErrorStatus i2cStatus;

void main()
{
	StwdStart();
  	InitSystem();
	InitFlash();
	RfInit();
	DebugLedStart();
	AlarmInit();
	LedSegmentInit();
  	OpenUartPort(UART_PORT_1, 9600);
	i2cStatus = SensorInit();
	if (i2cStatus == SUCCESS)
		MeasureProcess(NULL);
  	InitDevice(NULL);
	InitRegister();
	InitParameter(DefineParamProc);
	RegisterDeviceCallback(DEVICE_REGISTER_EVENT, OnDeviceRegisterProc);
	RegisterParamCallback(PARAM_CHANGE_EVENT, ParamChangeProc);	
  	while(1)
  	{
	  	ProcessUartEvents();
		ProcessTimerEvents();
  	}
}
