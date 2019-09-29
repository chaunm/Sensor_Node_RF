/********************************************************************************
Bkav
Product: Platform STM32F051
Module: Header file of flash module
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
#ifndef __FLASH_H__
#define __FLASH_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define HIGH_DENSITY

#ifdef LOW_DENSITY
#define FLASH_PAGE_FACTOR	10
#elif  MEDIUM_DENSITY
#define FLASH_PAGE_FACTOR	10
#elif defined HIGH_DENSITY
#define FLASH_PAGE_FACTOR	11
#endif // LOW_DENSITY
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID InitFlash();
VOID EraseFlash(DWORD nPage, DWORD nCount);
VOID WriteFlash(PVOID pData, WORD nSize, DWORD nAddr);
VOID ReadFlash(PVOID pBuffer, WORD nSize, WORD nAddr);
VOID SetBank(BYTE nBank);
PVOID FlashToMemory(DWORD nAddr);

#endif //__FLASH_H__

