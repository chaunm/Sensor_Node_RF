#include "stm32f0xx.h"
#include "timer.h"
#include "sensor.h"
#include "device-desc.h"
#include "device.h"
#include "parameter.h"
#include "measure.h"
#include "alarm.h"
#include "led-segment.h"

static ErrorStatus i2cStatus;
uint16_t humi;
int16_t temp;
static uint16_t oldHumi;
static int16_t oldTemp;

VOID MeasureProcess(PVOID pData)
{
	oldTemp = temp;
	oldHumi = humi;
	i2cStatus = SensorGetData(&temp, &humi);
	if (i2cStatus == SUCCESS)
	{
		TEMP_HIGH = (BYTE)(temp >> 8);
		TEMP_LOW = (BYTE)temp;
		HUMI_HIGH = (BYTE)(humi >> 8);
		HUMI_LOW = (BYTE)humi;
	}
	else		
	{
		temp = oldTemp;
		humi = oldHumi;
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
	if ((oldTemp != temp) || (oldHumi != humi))
		LedSegmentDisplayMeasureValue(temp, humi);
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
