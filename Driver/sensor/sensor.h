#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "hts221.h"
#include "sht30.h"

#ifdef SENSOR_HTS221
#define SensorInit()					HTS221_Init()
#define SensorReadTemperature(value)	HTS221_GetTemperature(value)
#define SensorReadHumidity(value)		HTS221_GetHumidity(value)
#define SeosorGetData(temp, humi)		HTS221_GetData(temp, humi)
#endif

#ifdef SENSOR_SHT30
#define SensorInit()					SHT3X_Init()
#define SensorReadTemperature(value)	SHT3X_GetTemperature(value)
#define SensorReadHumidity(value)		SHT3X_GetHumidity(value)
#define SensorGetData(temp, humi)		SHT3X_GetData(temp, humi);
#endif
#endif // __SENSOR_H__