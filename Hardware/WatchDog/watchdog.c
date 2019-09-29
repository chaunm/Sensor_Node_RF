/********************************************************************************  
 
Product: SHTMS
 
Module: WatchDog.c
 
Version: 1.0 
 
Author: NGUYEN KIM THANH
 
Created: 08-2014 
 
Modified:  
  <Name> 
  <Date> 
  <Change> 
 
Released: 
 
Description: Including functions for Watchdog
 
Note: <Note> 
 
********************************************************************************/ 

#include "watchdog.h"

/*------------------------------------------------------------------------------- 
Function: VOID WatchDog_Init(VOID);
 
Purpose: Initial Watchdog
 
Parameters: byBCD
 
Return: (byLow + byHigh)
 
Comments:  Watchdog 6.5s
 
Modified: 
  <Modified by> 
  <Date> 
  <Change> 
--------------------------------------------------------------------------------*/
VOID WatchDog_Init(VOID)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);
    IWDG_SetReload(0x0FFF);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

