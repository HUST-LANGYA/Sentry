#ifndef __DEV_ENCODER_H
#define __DEV_ENCODER_H

#define START_POS			-10000
//#define FIRST_CORNER  0		//2315 - 30cm
//#define SECOND_CORNER 0
#define END_POS				10000

#define TICK_PER_ROUND   1024 //先随便定一个，等上滑轨再读实际的数据
//void LIMIT_SWITCH_Configuration(void);
//void ENCODER_Configration(void);

//int32_t GetJourney(void);
int32_t get_pos(void);
void Encoder_detect(void);

#endif //__DEV_ENCODER_H
