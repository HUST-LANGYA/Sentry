#include "main.h"
/*--------------------------内部变量----------------------------*/

/*--------------------------外部引用变量----------------------------*/
extern gyro_Typedef Gyro_White;
extern gimbal_motor_t MotoPitch, MotoYaw;
extern _2006_motor_t BodanMotor;
/*--------------------------函数部分----------------------------*/

static float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);

/**
  * @brief  过零检测任务
  * @param  None
  * @retval None
  */
void task_ZeroCheck(void)
{
    // ! 好像过零检测结构体也要初始化一下的样子？ 
    // ! 记不太清楚了已经（当时这里就没有学清楚话说）
    // ! 好像过零检测不能开太快（vTaskDelay 1 好像不行）
    while (1)
    {
        MotoPitch.Angle_Inc = ZeroCheck(&MotoPitch.zerocheck, MotoPitch.Angle_ABS, Position);        //云台PITCH电机
        MotoYaw.Angle_Inc = ZeroCheck(&MotoYaw.zerocheck, MotoYaw.Angle_ABS, Position);              //云台YAW电机
        BodanMotor.Angle_Inc = ZeroCheck(&BodanMotor.zerocheck, BodanMotor.Angle_ABS, Position);     //拨弹电机
        Gyro_White.YAW_INC = ZeroCheck(&Gyro_White.zerocheck_yaw, Gyro_White.YAW_ABS, Position);     //陀螺仪YAW轴输出值
        Gyro_White.PITCH_INC = ZeroCheck(&Gyro_White.zerocheck_pitch, Gyro_White.PITCH, Position);   //陀螺仪PITCH输出值

        vTaskDelay(5);
    }
}


// !  inline 是C99之后的编译器才支持的好像 
// ! 如果keil编译出错的话 就把inline的 改成带参宏函数吧
// ! 到时候这些小函数 应该都要改成任务间通信用的信号量了
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
  * @brief  位置式和速度式过零检测
             Zero->ActualValue 表示检测量当前值
             Zero->LastValue 表示检测量上一次值
             Zero->CountCycle 表示检测量过零时越变值，即计数周期
             Zero->PreError 表示检测量差值
             使用此函数前要声明明对应检测量结构体的 Zero->CountCycle与Zero->LastValue
  * @param  ZeroCheck_Typedef *Zero  过零检测结构体
  *         float value  待检测量
            short Zerocheck_mode：取值Position或Speed
  * @retval 取决于Zerocheck_mode，分别输出过零检测后位置值或速度值
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

