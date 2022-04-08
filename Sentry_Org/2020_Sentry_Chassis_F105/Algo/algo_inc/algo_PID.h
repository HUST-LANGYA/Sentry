#ifndef __ALGO_PID_H
#define __ALGO_PID_H

typedef struct PID{
		float SetPoint;			//�趨Ŀ��ֵ
		
		float P;						//��������
		float I;						//���ֳ���
		float D;						//΢�ֳ���
		
		float LastError;		//ǰ�����
		float PreError;			//��ǰ���
		float SumError;			//�������
		float dError;
	
		float IMax;					//��������
		
		float POut;					//�������
		float IOut;					//�������
		float DOut;					//΢�����
}PID_Typedef;

float PID_Calc(PID_Typedef * P, float ActualValue);

#endif //__ALGO_PID_H