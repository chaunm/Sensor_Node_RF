#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "hts221.h"

#ifdef SENSOR_HTS221
#define SensorInit()					HTS221_Init()
#define SensorReadTemperature(value)	HTS221_GetTemperature(value)
#define SensorReadHumidity(value)		HTS221_GetHumidity(value)
#endif

#endif // __SENSOR_H__