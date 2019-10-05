#ifndef __LED_SEGMENT_H__
#define __LED_SEGMENT_H__

#define BUZZER_PORT			P2
#define BUZZER_DIR			P2DIR
#define MASK_BUZZER_PIN		MASK_BIT1
#define BUZZER_PIN			P2_1 

VOID LedSegmentInit();
VOID LedSegmentUpdateData(PBYTE pData, BYTE nLength);
VOID LedSegmentDisplayMeasureValue(int16_t temp, uint16_t humi);

#endif //__LED_SEGMENT_H__