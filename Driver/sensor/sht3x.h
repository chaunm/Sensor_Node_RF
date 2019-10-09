#ifndef __SHT3X_H__
#define __SHT3X_H__

#pragma pack(1)
typedef struct stSHT3XDATA
{
    WORD temp;
    BYTE nTempCrc;
    WORD Humi;
    BYTE nHumiCrc;
} SHT3XDATA, *PSHT3XDATA;

#define SHT3X_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SHT3X_LONG_TIMEOUT         ((uint32_t)(10 * SHT3X_FLAG_TIMEOUT)) 

ErrorStatus SHT3x_Init();
ErrorStatus SHT3X_GetTemperature(int16_t* value);
ErrorStatus SHT3X_GetHumidity(uint16_t* value);

#endif