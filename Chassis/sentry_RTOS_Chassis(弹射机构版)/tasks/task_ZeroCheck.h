#ifndef __TASK_ZERO_CHECK_H
#define __TASK_ZERO_CHECK_H

#define Position 1
#define Speed    2

typedef struct 
{
	float Circle;           //ת��Ȧ��
	float CountCycle;       //ת��һȦ���ܼ�������
	float LastValue;        //����������һ�ε�ֵ	
	float ActualValue;      //����������ǰֵ
	float PreError;         //������жϲ�ֵ
}ZeroCheck_Typedef;

void task_ZeroCheck(void);
//float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);
//void ZeroCheck_cal(void);
//void ZeroCheck_Init(void);

#endif //__TASK_ZERO_CHECK_H
