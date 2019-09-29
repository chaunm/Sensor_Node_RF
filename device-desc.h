#ifndef __DEVICE_DESC_H__
#define __DEVICE_DESC_H__

#define PARAM_COUNT			4

#define	MIN_TEMP_PARAM		1
#define MAX_TEMP_PARAM		2
#define MIN_HUMI_PARAM		3
#define MAX_HUMI_PARAM		4

#define MIN_TEMP	        PARAMETER(0)
#define MAX_TEMP	        PARAMETER(1)
#define MIN_HUMI	        PARAMETER(2)
#define MAX_HUMI		    PARAMETER(3)

#define DEFAULT_MIN_TEMP	0
#define DEFAULT_MAX_TEMP	40
#define DEFAULT_MIN_HUMI	0
#define	DEFAULT_MAX_HUMI	80

#define TEMP_HIGH_REG		16
#define TEMP_LOW_REG		17
#define HUMI_HIGH_REG		18
#define HUMI_LOW_REG		19
#define ALARM_REG			20

#define TEMP_HIGH			REGISTERS[TEMP_HIGH_REG]
#define TEMP_LOW			REGISTERS[TEMP_LOW_REG]
#define HUMI_HIGH			REGISTERS[HUMI_HIGH_REG]
#define HUMI_LOW			REGISTERS[HUMI_LOW_REG]
#define ALARM				REGISTERS[ALARM_REG]

#endif