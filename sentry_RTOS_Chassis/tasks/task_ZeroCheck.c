#include "main.h"
/*--------------------------�ڲ�����----------------------------*/

/*--------------------------�ⲿ���ñ���----------------------------*/

/*--------------------------��������----------------------------*/

static float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);

void task_ZeroCheck(void)
{
    //�����������ִ�й���������
    extern _3508_motor_t motor_chassis;
    
    //������ṹ��ĳ�ʼ��
    while(1)
    {
        motor_chassis.angle_inc = ZeroCheck(&motor_chassis.zerocheck,motor_chassis.angle_abs,Position);
		//��ȻĿǰ��û��������������Ҹо���������ֵӦ��ҲҪ�������������
//		encoder.angle_inc = ZeroCheck(&encoder.zerocheck,encoder.angle_abs ���� Read_Encoder(),Position);

        vTaskDelay(1);
    }
}



/**
  * @brief  λ��ʽ���ٶ�ʽ������
	           Zero->ActualValue ��ʾ�������ǰֵ
			   Zero->LastValue ��ʾ�������һ��ֵ
			   Zero->CountCycle ��ʾ���������ʱԽ��ֵ������������
			   Zero->PreError ��ʾ�������ֵ
			   ʹ�ô˺���ǰҪ������Ӧ������ṹ��� Zero->CountCycle��Zero->LastValue
  * @param  ZeroCheck_Typedef *Zero  ������ṹ��
  *         float value  �������
            int16_t Zerocheck_mode��ȡֵPosition��Speed
  * @retval ȡ����Zerocheck_mode���ֱ�����������λ��ֵ���ٶ�ֵ
  */
static float ZeroCheck(ZeroCheck_Typedef *Zero, float value, int16_t Zerocheck_mode)
{
	Zero->ActualValue = value;

	Zero->PreError = Zero->ActualValue - Zero->LastValue;
	Zero->LastValue = Zero->ActualValue;

	if (Zero->PreError > 0.7f * Zero->CountCycle)
	{
		Zero->PreError = Zero->PreError - Zero->CountCycle;
		Zero->Circle--;
	}
	if (Zero->PreError < -0.7f * Zero->CountCycle)
	{
		Zero->PreError = Zero->PreError + Zero->CountCycle;
		Zero->Circle++;
	}

	if (Zerocheck_mode == Position)
		return Zero->ActualValue + Zero->Circle * Zero->CountCycle;
	else if (Zerocheck_mode == Speed)
		return Zero->PreError;
	else
		return 0;
}

