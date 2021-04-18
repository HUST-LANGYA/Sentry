#include "main.h"
/*--------------------------�ڲ�����----------------------------*/

/*--------------------------�ⲿ���ñ���----------------------------*/
extern gyro_Typedef Gyro_White;
extern gimbal_motor_t MotoPitch,MotoYaw;
extern _2006_motor_t BodanMotor;
/*--------------------------��������----------------------------*/

/**
  * @brief  λ��ʽ���ٶ�ʽ������
	         Zero->ActualValue ��ʾ�������ǰֵ
			 Zero->LastValue ��ʾ�������һ��ֵ
			 Zero->CountCycle ��ʾ���������ʱԽ��ֵ������������
			 Zero->PreError ��ʾ�������ֵ
			 ʹ�ô˺���ǰҪ��������Ӧ������ṹ��� Zero->CountCycle��Zero->LastValue
  * @param  ZeroCheck_Typedef *Zero  ������ṹ��
  *         float value  �������
            short Zerocheck_mode��ȡֵPosition��Speed
  * @retval ȡ����Zerocheck_mode���ֱ�����������λ��ֵ���ٶ�ֵ
  */
float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode)
{
	Zero->ActualValue=value;
	
	Zero->PreError=Zero->ActualValue-Zero->LastValue;
	Zero->LastValue=Zero->ActualValue;
	
	if(Zero->PreError>0.7f*Zero->CountCycle)
	{
		Zero->PreError=Zero->PreError-Zero->CountCycle;
		Zero->Circle--;
	}
	if(Zero->PreError<-0.7f*Zero->CountCycle)
	{
		Zero->PreError=Zero->PreError+Zero->CountCycle;
		Zero->Circle++;
	}
	
	if(Zerocheck_mode==Position)
		return Zero->ActualValue + Zero->Circle*Zero->CountCycle;
	else if(Zerocheck_mode==Speed)
	  return Zero->PreError;
	else 
		return 0;
}

/**
  * @brief  ������
  * @param  None
  * @retval None
  */
void ZeroCheckHandle()
{
	MotoPitch.Angle_Inc = ZeroCheck(&MotoPitch.zerocheck,MotoPitch.Angle_ABS,Position);
	MotoYaw.Angle_Inc = ZeroCheck(&MotoYaw.zerocheck,MotoYaw.Angle_ABS,Position);
	BodanMotor.Angle_Inc = ZeroCheck(&BodanMotor.zerocheck,BodanMotor.Angle_ABS,Position);
	Gyro_White.YAW_INC = ZeroCheck(&Gyro_White.zerocheck_yaw,Gyro_White.YAW_ABS,Position);
	Gyro_White.PITCH_INC = ZeroCheck(&Gyro_White.zerocheck_pitch,Gyro_White.PITCH,Position);
}

float PitchAngleOutPut(void)
{
	return MotoPitch.Angle_Inc;
}

float YawAngleOutPut(void)
{
	return MotoYaw.Angle_Inc;
}

float GyroPitchOutPut(void)
{
	return Gyro_White.PITCH_INC;
}

float GyroYawOutPut(void)
{
	return Gyro_White.YAW_INC;
}



