#include "device.h"
#include "device-registers.h"
#include "device-desc.h"
#include "alarm.h"

VOID InitRegister()
{
	TEMP_HIGH = 0;
	TEMP_LOW = 0;
	HUMI_HIGH = 0;
	HUMI_LOW = 0;
	ALARM = 0;
	TEMP_ALARM = 0;
	HUMI_ALARM = 0;
	ALARM_ENABLE = 1;
}

VOID OnDeviceRegisterProc(PVOID pEvent)
{
	PREGISTEREVENT pRegisterEvent = (PREGISTEREVENT)pEvent;
	switch (pRegisterEvent->nRegister)
	{
	case ALARM_ENABLE_REG:
		if ((ALARM == 1) && (pRegisterEvent->nValue == 1))
		{
			AlarmOn();
		}
		else
		{
			AlarmOff();
		}		
		break;
	default:
	  	break;
	}
}