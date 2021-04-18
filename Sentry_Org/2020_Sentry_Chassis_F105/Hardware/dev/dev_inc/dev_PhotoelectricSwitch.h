#ifndef __BSP_LASER_H
#define __BSP_LASER_H

#define PHOTOELECTRIC_SWITCH_L GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) 
#define PHOTOELECTRIC_SWITCH_R GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) 

void photoelectricSwitch_detect(void);		//laser_r=18733	laser_l = -14684

//void Laser_Configuration(void);
//void Blink(int16_t BlinkTimes, int16_t LED_ID);
//void LED_Init(void);

#endif //__BSP_LASER_H
