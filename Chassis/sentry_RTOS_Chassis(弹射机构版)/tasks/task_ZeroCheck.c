#include "main.h"

static float ZeroCheck(ZeroCheck_Typedef *Zero,float value,int16_t Zerocheck_mode);

void task_ZeroCheck(void)
{
    //这个函数里面执行过零检测任务
    extern _3508_motor_t motor_chassis;
	  extern _2006_motor_t motor_gearbox;
    //extern _encoder_t  encoder;
    
    //过零检测结构体的初始化
    while(1)
    {
        motor_chassis.angle_inc = ZeroCheck(&motor_chassis.zerocheck,motor_chassis.angle_abs,Position);
			  motor_gearbox.angle_inc = ZeroCheck(&motor_gearbox.zerocheck,motor_gearbox.angle_abs,Position);
		//encoder.angle_inc = ZeroCheck(&encoder.zerocheck,encoder.angle_abs,Position);

        vTaskDelay(1);
    }
}



/**
  * @brief  位置式和速度式过零检测
	           Zero->ActualValue 表示检测量当前值
			   Zero->LastValue 表示检测量上一次值
			   Zero->CountCycle 表示检测量过零时越变值，即计数周期
			   Zero->PreError 表示检测量差值
			   使用此函数前要申明对应检测量结构体的 Zero->CountCycle与Zero->LastValue
  * @param  ZeroCheck_Typedef *Zero  过零检测结构体
  *         float value  待检测量
            int16_t Zerocheck_mode：取值Position或Speed
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

