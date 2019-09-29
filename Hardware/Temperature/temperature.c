/******************************************************************************** 
Bkav
Product: Platform STM32F051
Module: 
Version: 1.0
Author: ThiVD
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "hw_adc.h"
#include "hw_rcc.h"
#include "temperature.h"
/*-----------------------------------------------------------------------------*/
/* Local Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Local Macro definitions */
/*-----------------------------------------------------------------------------*/
#define TEMP30_CAL_ADDR ((PWORD) ((DWORD)0x1FFFF7B8))
#define TEMP110_CAL_ADDR ((PWORD) ((DWORD)0x1FFFF7C2))

#define VDD_SUPPLY      300
#define VDD_CALIB       330
/*-----------------------------------------------------------------------------*/
/* Local Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Global variables */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function implementations */
/*-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------- 
Function:  VOID InitInternalTemperature()
Purpose: 
Parameters: None
Return: None
Comments:  None
Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
VOID InitInternalTemperature()
{
    ADC_InitTypeDef AdcConfig;
    //Enable ADC1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // ADC Deinit
    ADC_DeInit(ADC1);
    // Initialize ADC struct config
    ADC_StructInit(&AdcConfig);
    // Config ADC1
    AdcConfig.ADC_ContinuousConvMode = DISABLE;
    AdcConfig.ADC_DataAlign = ADC_DataAlign_Right;
    AdcConfig.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    AdcConfig.ADC_Resolution = ADC_Resolution_12b;
    ADC_Init(ADC1, &AdcConfig);
    // Enable temp sensor
    ADC_TempSensorCmd(ENABLE);
    // Config Sample time
    ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor, ADC_SampleTime_239_5Cycles);
    // ADC Calibration
    ADC_GetCalibrationFactor(ADC1);
    // Eanble ADC1
    ADC_Cmd(ADC1, ENABLE);
    // wait for ADC1 to be ready
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));   
}
/*------------------------------------------------------------------------------- 
Function:  int32_t GetTemperature()
Purpose: 
Parameters: None
Return: None
Comments:  None
Modified: 
<Modified by> 
<Date> 
<Change> 
--------------------------------------------------------------------------------*/
int32_t GetTemperature()
{
    int32_t dwTemp = 0;
    ADC_StartOfConversion(ADC1);
    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    dwTemp = (int32_t)(ADC_GetConversionValue(ADC1));
    dwTemp = (dwTemp * VDD_SUPPLY / VDD_CALIB) - ((int32_t)*TEMP30_CAL_ADDR);
    dwTemp = dwTemp * (int32_t)(110 - 30);
    dwTemp = dwTemp / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
    return (dwTemp + 30);
}
