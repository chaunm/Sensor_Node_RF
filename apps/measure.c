#include "stm32f0xx.h"
#include "timer.h"
#include "sensor.h"
#include "device-desc.h"
#include "device.h"
#include "parameter.h"
#include "measure.h"
#include "alarm.h"

static ErrorStatus i2cStatus;
static uint16_t humi;
static int16_t temp;

VOID MeasureProcess(PVOID pData)
{
	i2cStatus = SensorReadTemperature(&temp);
	if (i2cStatus == SUCCESS)
	{
		TEMP_HIGH = (BYTE)(temp >> 8);
		TEMP_LOW = (BYTE)temp;
	}
	
	i2cStatus = SensorReadHumidity(&humi);
	if (i2cStatus == SUCCESS)
	{
		HUMI_HIGH = (BYTE)(humi >> 8);
		HUMI_LOW = (BYTE)humi;
	}
	
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
	StartShortTimer(MEASURE_PERIOD_MS, MeasureProcess, NULL);
}

uint16_t GetHumidity()
{
	return humi;
}

int16_t GetTemperature()
{
	return temp;
}
