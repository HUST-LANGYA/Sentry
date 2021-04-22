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
  delay_ms(600); //随便延时一下
  FrictionWheel_SetSpeed(400, 400);
  delay_ms(1000);
  FrictionWheel_SetSpeed(750, 750);
}

//改变占空比，设置摩擦轮转速
void FrictionWheel_SetSpeed(uint16_t tmpAccelerator0, uint16_t tmpAccelerator1)
{
  //限幅
  accelerator0 = LIMIT_MAX_MIN(tmpAccelerator0, 900, 400);
  accelerator1 = LIMIT_MAX_MIN(tmpAccelerator1, 900, 400);
  //这里放成变量是方便调试时观察
  //赋值
  // TIM_SetCompare1(TIM8,accelerator0);
  // TIM_SetCompare2(TIM8,accelerator1);

  //2006的话，就算PID把电流值用CAN发过去
  extern _2006_motor_t FrictionMotor[2];
  FrictionMotor[0].pid_speed.SetPoint = accelerator0;
  FrictionMotor[1].pid_speed.SetPoint = accelerator1;

  float fsend;

  fsend = PID_Calc(&FrictionMotor[0].pid_speed, FrictionMotor[0].RealSpeed, 0); //用速度环算电流值输出
  FrictionMotor[0].I_Set = (int16_t)LIMIT_MAX_MIN(fsend, FrictionCurrentLimit, -FrictionCurrentLimit);
  fsend = PID_Calc(&FrictionMotor[1].pid_speed, FrictionMotor[1].RealSpeed, 0); //用速度环算电流值输出
  FrictionMotor[1].I_Set = (int16_t)LIMIT_MAX_MIN(fsend, FrictionCurrentLimit, -FrictionCurrentLimit);

  Friction_Can1Send(FrictionMotor[0].I_Set, FrictionMotor[1].I_Set); //发拨弹电流
}
