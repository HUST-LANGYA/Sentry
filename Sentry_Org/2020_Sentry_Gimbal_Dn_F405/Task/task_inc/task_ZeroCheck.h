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
}
ZeroCheck_Typedef;

float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);
void ZeroCheckHandle(void);

float PitchAngleOutPut(void);
float GyroPitchOutPut(void);

float YawAngleOutPut(void);
float GyroYawOutPut(void);

#endif //__TASK_ZERO_CHECK_H

