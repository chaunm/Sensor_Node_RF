#ifndef __MEASURE_H__
#define __MEASURE_H__

#define MEASURE_PERIOD_MS	5000

VOID MeasureProcess(PVOID pData);
uint16_t GetHumidity();
int16_t GetTemperature();

#endif // __MEASURE_H__