#include "stm32f0xx.h"
#include "timer.h"
#include "sensor.h"
#include "device-desc.h"
#include "device.h"
#include "parameter.h"
#include "measure.h"
#include "alarm.h"
#include "led-segment.h"
#include "stwd100.h"

#define TEST_COUNT	2
static ErrorStatus i2cStatus;
uint16_t humi;
int16_t temp;
static uint16_t oldHumi;
static int16_t oldTemp;
static uint8_t testCount = 0;
static uint8_t test;
static uint16_t testValue;
static uint8_t testFinish = 0;

VOID MeasureProcess(PVOID pData)
{
	oldTemp = temp;
	oldHumi = humi;
	StwdManualToggle();
	i2cStatus = SensorGetData(&temp, &humi);
	StwdManualToggle();
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
	if ((temp > MAX_TEMP) || (temp < MIN_TEMP))
	{
		TEMP_ALARM = 1;
	}
	else
	{
		TEMP_ALARM = 0;
	}
	if ((humi > MAX_HUMI) || (humi < MIN_HUMI))
	{
		HUMI_ALARM = 1;
	}
	else
	{
		HUMI_ALARM = 0;
	}
	if ((TEMP_ALARM ==  1) || (HUMI_ALARM == 1))
	{
		ALARM = 1;
		if (ALARM_ENABLE == 1)
			AlarmOn();
	}
	else 
	{
		ALARM = 0;
		AlarmOff();
	}
	if (testCount < TEST_COUNT)
	{
		testValue = (uint16_t)test * 111;
		LedSegmentDisplayMeasureValue(testValue, testValue);
		test++;
		if (test == 10)
		{
			testCount++;
			test = 0;
			if (testCount == TEST_COUNT)
				testFinish = 1;
		}
		StartShortTimer(TEST_DISPLAY_PERIOD_MS, MeasureProcess, NULL);
		return;
	}
	if ((oldTemp != temp) || (oldHumi != humi) || (testFinish == 1))
	{
		LedSegmentDisplayMeasureValue(temp, humi);
		testFinish = 0;
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
