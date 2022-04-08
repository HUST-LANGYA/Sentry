#ifndef __BSP_GPIO_LIMIT_SWITCH_H
#define __BSP_GPIO_LIMIT_SWITCH_H

#define LimitSw_TIMEOUT 150   //触发超时的时限
#define LimitSw_L GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)   //左行程开关的读数
#define LimitSw_R GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)  //右行程开关的读数

void GPIO_Config_Limit_Switch(void);

//定义行程开关读取的返回状态(记得试一遍空闲状态，因为行程开关的常开常闭会不一样)
#define LimitSw_L_Idle  1   //低电平表示空闲
#define LimitSw_L_Active   0   //高电平表示触发（￣︶￣）↗　

#define LimitSw_R_Idle  1   //低电平表示空闲
#define LimitSw_R_Active   0   //高电平表示触发（￣︶￣）↗　


//void limitSwitch_detect(void);
#endif //__BSP_GPIO_LIMIT_SWITCH_H
