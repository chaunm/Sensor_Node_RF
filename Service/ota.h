#ifndef __OTA_H__
#define __OTA_H__
#include "types.h"

#define FIRST_BLOCK 0x55
#define LAST_BLOCK  0xAA

enum FlashStatus {
    FLASH_SUCCESS = 0,
    FLASH_FAILED
};

#define FLASH_RESPONSE_LENGTH   4
/* max RF size should be 58 bytes
Flash package data format:
0xAA length type nAddress(2 bytes) package_index(2 bytes) flag(1byte) lenght(1byte) data(40byte max) 0x55

OTA return message
0xAA length type nAddress(2 bytes) status 0x55
*/



typedef struct tagFLASHPACKAGE {
    WORD nPackageIndex;
    BYTE flag;
    BYTE nLength;
    PBYTE pData;
} FLASHPAGKAGE, *PFLASHPACKAGE;

typedef struct tagFLASHRESPONSE {
    BYTE nLength;
    BYTE nType;
    WORD nAddr;
    BYTE status; 
} FLASHRESPONSE, *PFLASHRESPONSE;

VOID Ota_ProcessFlashPackage(PUARTBUFFER pBuffer)
#endif // __OTA_H__
