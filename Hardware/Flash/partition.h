#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "stm32f0xx_flash.h"
#define SECTOR_SIZE         0x1000

#ifdef STM32F030
/* BOOT LOADER SIZE - 2K */
#define BOOTLOADER_START_ADDR       0x00000000
#define BOOTLOADER_END_ADDR         0x000007FF
#define BOOTLOADER_SIZE             0x00000800
#define BOOT_LOADER_START_PAGE		0
#define BOOT_LOADER_END_PAGE		1

/* APPLICATION SIZE 14KB: 0x00000800 - 0x00003FFF */
#define APPLICATION_START_ADDR      0x00000800
#define APPLICATION_END_ADDR        0x00003FFF
#define APPLICATION_SIZE            0x00003800
#define APPLICATION_START_PAGE		2
#define APPLIACTION_END_PAGE		15

/* IMAGE SECTOR 14KB: 0x00004000 - 0x00007FFF */
#define IMAGE_START_ADDR            0x00004000
#define IMAGE_END_ADDR              0x000077FF
#define IMAGE_SIZE                  0x00003800
#define IMAGE_START_PAGE			16
#define IMAGE_END_PAGE				29

#define INFORMATION_START_ADDR      0x00007800
#define INFORMATION_END_ADDR        0x000007FF
#define INFORMATION_SIZE            0x00008000
#define INFORMATION_START_PAGE		30
#define INFORMATION_END_PAGE		31

/* Copy buffer size must be divided by IMAGE_SIZE and less than 248KB */
#define COPY_BUFFER_SIZE    2048
#define FLASH_WRITE_ELEMENT 7

#define APPLICATION_MEM_ADDRESS		0x08000800
#endif

#ifdef STM32F091
/* BOOT LOADER SIZE - 4K */
#define BOOTLOADER_START_ADDR       0x00000000
#define BOOTLOADER_END_ADDR         0x00000FFF
#define BOOTLOADER_SIZE             0x00001000
#define BOOT_LOADER_START_PAGE		0
#define BOOT_LOADER_END_PAGE		1

/* APPLICATION SIZE 14KB: 0x00000800 - 0x00003FFF */
#define APPLICATION_START_ADDR      0x00001000
#define APPLICATION_END_ADDR        0x0001FFFF
#define APPLICATION_SIZE            0x0001F000
#define APPLICATION_START_PAGE		2
#define APPLIACTION_END_PAGE		63

/* IMAGE SECTOR 14KB: 0x00004000 - 0x00007FFF */
#define IMAGE_START_ADDR            0x00020000
#define IMAGE_END_ADDR              0x0003EFFF
#define IMAGE_SIZE                  0x0001F000
#define IMAGE_START_PAGE			64
#define IMAGE_END_PAGE				125

#define INFORMATION_START_ADDR      0x0003F000
#define INFORMATION_END_ADDR        0x00000FFF
#define INFORMATION_SIZE            0x00001000
#define INFORMATION_START_PAGE		126
#define INFORMATION_END_PAGE		127

/* Copy buffer size must be divided by IMAGE_SIZE and less than 248KB */
#define COPY_BUFFER_SIZE    		4096
#define FLASH_WRITE_ELEMENT 		31

#define APPLICATION_MEM_ADDRESS		0x08001000
#endif


#endif
