#ifndef __BSP_TIM3_ENCODER_H
#define __BSP_TIM3_ENCODER_H

//#define START_POS			-10000
//#define FIRST_CORNER  0		//2315 - 30cm
//#define SECOND_CORNER 0
//#define END_POS				10000

// void TIM3_Configuration_Limit_Switch(void);
void TIM3_Config_Encoder(void);
/*inline*/ uint16_t Read_Encoder(void);

//int32_t GetJourney(void);
//void Encoder_detect(void);

#endif //__BSP_TIM3_ENCODER_H
