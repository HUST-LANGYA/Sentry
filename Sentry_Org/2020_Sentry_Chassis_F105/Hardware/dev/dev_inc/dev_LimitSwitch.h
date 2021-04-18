#ifndef __DEV_LIMIT_SWITCH_H
#define __DEV_LIMIT_SWITCH_H

#define KEY_TIMEOUT 150   //触发超时的时限
#define KEY_LEFT GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)   //左行程开关的读数
#define KEY_RIGHT GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)  //右行程开关的读数


void limitSwitch_detect(void);

#endif //__DEV_LIMIT_SWITCH_H
