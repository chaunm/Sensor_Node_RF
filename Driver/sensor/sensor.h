#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "hts221.h"

#ifdef SENSOR_HTS221
#define SensorInit()					HTS221_Init()
#define SensorReadTemperature(value)	HTS221_GetTemperature(value)
#define SensorReadHumidity(value)		HTS221_GetHumidity(value)
#endif

#ifdef SENSOR_SHT30
#define SensorInit()					SHT3X_Init()
#define SensorReadTemperature(value)	SHT3X_GetTemperature(value)
#define SensorReadHumidity(value)		SHT3X_GetHumidity(value)
#endif
#endif // __SENSOR_H__