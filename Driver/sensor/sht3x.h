#ifndef __SHT3X_H__
#define __SHT3X_H__

                                  //adr  command  r/w
#define SHT1_STATUS_REG_W 0x06   //000   0011    0
#define SHT1_STATUS_REG_R 0x07   //000   0011    1
#define SHT1_MEASURE_TEMP 0x03   //000   0001    1
#define SHT1_MEASURE_HUMI 0x05   //000   0010    1
#define SHT1_RESET        0x1e   //000   1111    0

#endif