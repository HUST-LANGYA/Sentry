#ifndef __DEV_LIMIT_SWITCH_H
#define __DEV_LIMIT_SWITCH_H

#define KEY_TIMEOUT 150   //������ʱ��ʱ��
#define KEY_LEFT GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)   //���г̿��صĶ���
#define KEY_RIGHT GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)  //���г̿��صĶ���


void limitSwitch_detect(void);

#endif //__DEV_LIMIT_SWITCH_H
