#ifndef __BSP_GPIO_PHOTOELECTRIC_SWITCH_H
#define __BSP_GPIO_PHOTOELECTRIC_SWITCH_H

//�궨��������翪�ص�״̬��ȡ
#define PhotoEleSw_R GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) 
#define PhotoEleSw_L GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) 

//�����翪�ض�ȡ�ķ���״̬(�ǵ���һ�����״̬)
#define PhotoEleSw_Active  0   //�͵�ƽ��ʾ����
#define PhotoEleSw_Idle    1   //�ߵ�ƽ��ʾ����

void GPIO_Config_Phtotelectric_Switch(void);
///*inline*/ void photoelectricSwitch_detect(void);		//laser_r=18733	laser_l = -14684

#endif //__BSP_GPIO_PHOTOELECTRIC_SWITCH_H
