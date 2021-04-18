#ifndef __BSP_GPIO_LIMIT_SWITCH_H
#define __BSP_GPIO_LIMIT_SWITCH_H

#define LimitSw_TIMEOUT 150   //触发超时的时限
#define LimitSw_L GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)   //左行程开关的读数
#define LimitSw_R GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)  //右行程开关的读数

void GPIO_Config_Limit_Switch(void);

void limitSwitch_detect(void);
#endif //__BSP_GPIO_LIMIT_SWITCH_H
