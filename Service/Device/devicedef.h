/********************************************************************************
Bkav
Product: 
Module: 
Version: 1.0
Author: 
Created: 4/2015
Modified: 
<Name>
<Date>
<Change>
Released: <Date>
Description: <Description>
Note: <Note>
********************************************************************************/
#ifndef __DEVICEDEF_H__
#define __DEVICEDEF_H__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
#define FLASH_DATA_SIZE 	        16
#define PAGE_MASK_SIZE		        16

typedef struct tagDEVICEREGISTERPART {
    BYTE nRegister;
    BYTE nValue;
} DEVICEREGISTERPART, *PDEVICEREGISTERPART;

typedef struct tagDEVICEREGISTER {
    union {
        struct {
            BYTE nRegister;
            BYTE nValue;
        };
        DEVICEREGISTERPART pPart[1];
    };
} DEVICEREGISTER, *PDEVICEREGISTER;

#pragma pack(1)
typedef struct tagDEVICEPARAMPART {
    WORD nParam;
    WORD nValue;
} DEVICEPARAMPART, *PDEVICEPARAMPART;

#pragma pack(1)
typedef struct tagDEVICEPARAM {
    union {
        struct {
            WORD nParam;
            WORD nValue;
        };
        DEVICEPARAMPART pPart[1];
    };
} DEVICEPARAM, *PDEVICEPARAM;

// Flash

typedef struct tagPAGEDATA {
    BYTE nIndex;
    BYTE pData[FLASH_DATA_SIZE];
} PAGEDATA, *PPAGEDATA;

#pragma pack(1)
typedef struct tagENDPAGE {
    BYTE nPage;
    WORD nCheck;
} ENDPAGE, *PENDPAGE;

#pragma pack(1)
typedef struct tagFINISHFLASH {
    BYTE nPage;	
    BYTE nCount;
    WORD nCheck;
} FINISHFLASH, *PFINISHFLASH;

typedef struct tagPAGEREADY {
    BYTE nPage;	
} PAGEREADY, *PPAGEREADY;
    
typedef struct tagPAGEMASK {
    BYTE nPage;
    BYTE pMask[PAGE_MASK_SIZE];
} PAGEMASK, *PPAGEMASK;
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define _PDEVICEREGISTER(buffer)    ((PDEVICEREGISTER)(buffer))
#define _PDEVICEPARAM(buffer)       ((PDEVICEPARAM)(buffer))
#define _PPAGEDATA(buffer)          ((PPAGEDATA)(buffer))
#define _PENDPAGE(buffer)           ((PENDPAGE)(buffer))
#define _PFINISHFLASH(buffer)       ((PFINISHFLASH)(buffer))

#define _PPAGEREADY(buffer) ((PPAGEREADY)(buffer))

#define MAKE_PAGE_READY_PACKAGE(buffer, page) { \
    _PPAGEREADY(buffer)->nPage = (page); \
}

#define _PPAGEMASK(buffer) ((PPAGEMASK)(buffer))

#define MAKE_PAGE_MASK_PACKAGE(buffer, page, mask) { \
    _PPAGEMASK(buffer)->nPage = (page); \
	CopyMemory(_PPAGEMASK(buffer)->pMask, mask, PAGE_MASK_SIZE); \
}

// Server packages
#define PACKAGE_TYPE_DEVICE_REGISTER	0x0E	// 14
#define PACKAGE_TYPE_DEVICE_PARAM	    0x0F	// 15
#define PACKAGE_TYPE_DEVICE_WIFI        0x0D    // 13

#define PAKAGE_TYPE_WIFI_COMMAND        0x01
#define PACKAGE_TYPE_DEVICE_FLASH       0x38
#define PACKAGE_TYPE_LOST_NETWORK       0x3C    // LOST NETWORK
#define PACKAGE_TYPE_GET_TICK           0x3D    // 50
#define PACKAGE_TYPE_REQUIRE_REGISTTER  0x3E    //51

// Packages for wifi devices
#define PACKAGE_TYPE_WIFI_RESET         0x01
#define PACKAGE_TYPE_WIFI_UPDATE        0x03    // 3

#define RESET_WIFI_MODULE               0x01
#define REFLASH_WIFI_MODULE             0x02
#define REFLASH_MCU                     0x03
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
#endif // __DEVICEDEF_H__