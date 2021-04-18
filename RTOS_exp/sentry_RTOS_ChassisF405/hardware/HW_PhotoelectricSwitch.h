#ifndef __BSP_GPIO_PHOTOELECTRIC_SWITCH_H
#define __BSP_GPIO_PHOTOELECTRIC_SWITCH_H

//宏定义两个光电开关的状态读取
#define PhotoEleSw_R GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) 
#define PhotoEleSw_L GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) 

//定义光电开关读取的返回状态(记得试一遍空闲状态)
#define PhotoEleSw_Active  0   //低电平表示触发
#define PhotoEleSw_Idle    1   //高电平表示空闲

void GPIO_Config_Phtotelectric_Switch(void);
///*inline*/ void photoelectricSwitch_detect(void);		//laser_r=18733	laser_l = -14684

#endif //__BSP_GPIO_PHOTOELECTRIC_SWITCH_H
