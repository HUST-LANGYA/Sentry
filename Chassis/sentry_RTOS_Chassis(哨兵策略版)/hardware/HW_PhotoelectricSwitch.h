#ifndef __BSP_GPIO_PHOTOELECTRIC_SWITCH_H
#define __BSP_GPIO_PHOTOELECTRIC_SWITCH_H


//�궨��������翪�ص�״̬��ȡ
//#define Ftir_L GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) 
//#define Ftir_R GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) 

//�����翪�ض�ȡ�ķ���״̬(�ǵ���һ�����״̬)
#define Ftir_Active  1   //�͵�ƽ��ʾ����
#define Ftir_Idle    0   //�ߵ�ƽ��ʾ����

void GPIO_Config_Phtotelectric_Switch(void);
///*inline*/ void photoelectricSwitch_detect(void);		//laser_r=18733	laser_l = -14684

#endif //__BSP_GPIO_PHOTOELECTRIC_SWITCH_H
