#ifndef __BSP_TIM2_TIMER_H
#define __BSP_TIM2_TIMER_H

void TIM2_Configuration_Timer(void);
int32_t GetTimNow(void);
void delay_us(int32_t n);
//void delay_us(int n);
#endif //__BSP_TIM2_TIMER_H
