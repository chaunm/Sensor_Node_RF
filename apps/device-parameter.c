#include "stm32f0xx.h"
#include "device-desc.h"
#include "device.h"
#include "parameter.h"
#include "measure.h"
#include "alarm.h"

	
VOID DefineParamProc(PVOID pParameter)
{
	DEFINE_PARAM(0, MIN_TEMP_PARAM, DEFAULT_MIN_TEMP);
	DEFINE_PARAM(1, MAX_TEMP_PARAM, DEFAULT_MAX_TEMP);
	DEFINE_PARAM(2, MIN_HUMI_PARAM, DEFAULT_MIN_HUMI);
	DEFINE_PARAM(3, MAX_HUMI_PARAM, DEFAULT_MAX_HUMI);
}

VOID ParamChangeProc(PVOID pParam)
{
	// temp or humi threshold change should check and update
	int16_t temp;
	uint16_t humi;
	temp = GetTemperature();
	humi = GetHumidity();
	if ((temp > MAX_TEMP) || (temp < MIN_TEMP) || (humi < MIN_HUMI) || (humi > MAX_HUMI))
	{
		if (ALARM == 0)
		{
			ALARM = 1;
			AlarmOn();
		}
	}
	else
	{
		if (ALARM == 1)
		{
			ALARM = 0;
			AlarmOff();
		}
	}
}