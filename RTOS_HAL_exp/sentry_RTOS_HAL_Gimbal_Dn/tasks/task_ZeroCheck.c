#include "main.h"
/*--------------------------�ڲ�����----------------------------*/

/*--------------------------�ⲿ���ñ���----------------------------*/
extern gyro_Typedef Gyro_White;
extern gimbal_motor_t MotoPitch, MotoYaw;
extern _2006_motor_t BodanMotor;
/*--------------------------��������----------------------------*/

/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void task_ZeroCheck(void)
{
    // ! ���������ṹ��ҲҪ��ʼ��һ�µ����ӣ� 
    // ! �ǲ�̫������Ѿ�����ʱ�����û��ѧ�����˵��
    while (1)
    {
        MotoPitch.Angle_Inc = ZeroCheck(&MotoPitch.zerocheck, MotoPitch.Angle_ABS, Position);        //��̨PITCH���
        MotoYaw.Angle_Inc = ZeroCheck(&MotoYaw.zerocheck, MotoYaw.Angle_ABS, Position);              //��̨YAW���
        BodanMotor.Angle_Inc = ZeroCheck(&BodanMotor.zerocheck, BodanMotor.Angle_ABS, Position);     //�������
        Gyro_White.YAW_INC = ZeroCheck(&Gyro_White.zerocheck_yaw, Gyro_White.YAW_ABS, Position);     //������YAW�����ֵ
        Gyro_White.PITCH_INC = ZeroCheck(&Gyro_White.zerocheck_pitch, Gyro_White.PITCH, Position);   //������PITCH���ֵ

        vTaskDelay(5); //ע������ӳٿ����е㳤�ˣ����Ҫ���������ֵ��PID�Ļ���������Ҫ������ӳٵ���1ms��2ms
    }
}


// !  inline ��C99֮��ı�������֧�ֵĺ��� 
// ! ���keil�������Ļ� �Ͱ�inline�� �ĳɴ��κ꺯����
// ! ��ʱ����ЩС���� Ӧ�ö�Ҫ�ĳ������ͨ���õ��ź�����
inline float PitchAngleOutPut(void)
{
    return MotoPitch.Angle_Inc;
}

inline float YawAngleOutPut(void)
{
    return MotoYaw.Angle_Inc;
}

inline float GyroPitchOutPut(void)
{
    return Gyro_White.PITCH_INC;
}

inline float GyroYawOutPut(void)
{
    return Gyro_White.YAW_INC;
}






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

