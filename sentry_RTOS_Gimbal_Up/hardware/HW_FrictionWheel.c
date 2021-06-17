#include "main.h"
/**
  * @brief  设置摩擦轮转速（油门值）
  * @param  两轮速度值
  * @retval None
  */
uint16_t accelerator0;
uint16_t accelerator1;
//extern uint16_t throttle;


//摩擦轮上电初始化
void FrictionWheel_Init(void)
{
     delay_ms(600);              //随便延时一下
     FrictionWheel_SetSpeed(400,400);
     delay_ms(1000);        
     FrictionWheel_SetSpeed(0,0);           
    
    //写到ARR里面的是1000，占空比就是 pulse/ARR * 100%
}

//改变占空比，设置摩擦轮转速
void FrictionWheel_SetSpeed(uint16_t tmpAccelerator0, uint16_t tmpAccelerator1)
{
    //限幅
    accelerator0=LIMIT_MAX_MIN(tmpAccelerator0,710,400);
    accelerator1=LIMIT_MAX_MIN(tmpAccelerator1,710,400);
    //这里放成变量是方便调试时观察
    //赋值
    TIM_SetCompare1(TIM8,accelerator0);
    TIM_SetCompare2(TIM8,accelerator1);
}

