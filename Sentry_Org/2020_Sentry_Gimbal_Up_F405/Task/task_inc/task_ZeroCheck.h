#ifndef __TASK_ZEROCHECK_H
#define __TASK_ZEROCHECK_H

#define Position 1
#define Speed    2

typedef struct 
{
	float Circle;           //转过圈数
	float CountCycle;       //转过一圈的总计数周期
	float LastValue;        //检测过零量上一次的值	
	float ActualValue;      //检测过零量当前值
	float PreError;         //检测量判断差值
}ZeroCheck_Typedef;

float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);
void ZeroCheckHandle(void);

float PitchAngleOutPut(void);
float GyroPitchOutPut(void);

float YawAngleOutPut(void);
float GyroYawOutPut(void);

#endif //__TASK_ZEROCHECK_H

