#include "device.h"
#include "device-registers.h"
#include "device-desc.h"

VOID InitRegister()
{
	TEMP_HIGH = 0;
	TEMP_LOW = 0;
	HUMI_HIGH = 0;
	HUMI_LOW = 0;
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