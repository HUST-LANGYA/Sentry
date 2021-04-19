#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

////功率参数
float PowerNow;
PID_Typedef PowerLimit;
float PowerLimit_k=0.8;



int16_t encoder_veloc = 0;
int32_t encoder_distance = 0;//用编码器记录到减速段之前的最大距离
void task_Chassis(void* parameter)
{
    //进入任务时执行一次复位
    PID_Chassis_Init(); //第一次执行底盘任务时 给底盘的PID参数进行初始化
    
    while (1)
    {
        HeatUpdate();   //计算枪口热量值，这个函数的输出值，会通过CAN发送到上下云台板上去
        ShootingHeat_CAN2Send();        
        encoder_distance = Read_Encoder();
        
        if (Sentry_State.Chassis_Mode == Chassis_Patrol) Chassis_Patrol_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_RC) Chassis_RC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_SLEEP) Chassis_SLEEP_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_DEBUG) Chassis_DEBUG_Act();
        
        vTaskDelay(1);
    }
}


/**
  * @brief  遥控模式下底盘的运动
  * @param  None
  * @retval None
  */
static void Chassis_RC_Act(void)
{
    static float kChassis = 4.0f;   //缩放系数
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    RC_Ctl_t RC_Ctl = getRCData();

//    //底盘+上云台遥控： 左摇杆控制云台，右摇杆控制底盘和发射
//    if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC /* && Sentry_State.ChassisMode & Chassis_RC*/)
//        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch0-1024/*1043*/); 
//    
//    //底盘+下云台遥控： 右摇杆控制云台，左摇杆控制底盘和发射
//    if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_RC /*&& Sentry_State.ChassisMode & Chassis_RC*/)
//        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch2-1024/*1002*/); 

    Chassis_SLEEP_PID_Cal();
}

/**
  * @brief  PC模式下底盘的运动
  * @param  None
  * @retval None
  */
uint16_t waitingTick;//在端头暂停的时间
uint32_t movingTick;//记录运动的时间
uint16_t movingDir;//当前行进的方向  1为向右 0为向左
static void Chassis_Patrol_Act(void)
{
    extern uint8_t startFlag, destinationFlag;
    extern uint8_t reverseFlag;

    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=1;
        chassisFlag_Init=1; 
    }
        
    extern PhotoEleSWState_t PhotoEle_State;  //光电开关状态
    
    
    //根据运动状态，做计数器计数
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
    {//两边都未触发
//        if(motor_chassis.pid.SetPoint)
//        {//若是正常运动，waitingTick清零，movingTick++
            waitingTick = 0;
            movingTick++;
//        }
//        else 
//        {
//           //若是人为暂停，则同时清零waitingTick和movingTick
//           waitingTick = 0;
//           movingTick = 0;
//        }
    }
    else if(PhotoEle_State == PhotoEle_Left_Active)  
    {//左侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++;
        movingTick = 0;
        movingDir = 1;
    }
    else if( PhotoEle_State == PhotoEle_Right_Active )
    {//右侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++;
        movingTick = 0;
        movingDir = 0;
    }
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
    {//双边触发，考虑为人为暂停情况
        waitingTick = 1;//报错！！！！
        movingTick = 0;
    }
    
    
    
    //根据计数值，改变运动状态
    if(waitingTick>0 && waitingTick<200 && movingTick == 0) 
    {//暂停状态0速
        motor_chassis.pid.SetPoint = 0;
    }
    else if(waitingTick>200 && movingTick == 0)
    {//从零启动的1速状态
        motor_chassis.pid.SetPoint = (movingDir)?(-1000):(1000);    
    }
    else
    {
        if(waitingTick == 0 && movingTick > 300)
        {//从1速开始再加一次速的2速状态
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);    
        }
        if(waitingTick == 0 && movingTick > 600)
        {//从1速开始再加一次速的2速状态
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);    
        }
        if(waitingTick == 0 && movingTick > 1000)
        {//从2速开始再加一次速的3速状态
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);    
        }
//        if(waitingTick == 0 && movingTick > 2000)
//        {//从3速开始再加一次速的4速状态
//            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);    
//        }
    }
    
    Chassis_Patrol_PID_Cal();
}


static void Chassis_SLEEP_Act(void)
{
    //可能要设置一些flag在这里
    Chassis_SLEEP_PID_Cal();
}

int16_t debug_SetPoint = 0;
static void Chassis_DEBUG_Act(void)
{   
    motor_chassis.pid.SetPoint = debug_SetPoint;
    Chassis_DEBUG_PID_Cal();
}

static void Chassis_RC_PID_Cal(void)
{
    float Chassis_Iset;
    
    //底盘3508 限幅+赋值
    motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = PID_Calc( &motor_chassis.pid, motor_chassis.real_speed);
//    PowerLimit_k += PID_Calc(&PowerLimit, PowerNow) / PowerLimit.SetPoint;
//    PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k, 1, 0);
    Chassis_Iset = LIMIT_MAX_MIN(Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
//  Chassis_Iset = PowerLimit_k * Chassis_Iset;
    
    Chassis_CAN1Send(Chassis_Iset);
}

static void Chassis_Patrol_PID_Cal(void)
{
    float Chassis_Iset;
    extern INA260 INA260_1;
//  motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint, ChassisCurrentLimit, -ChassisCurrentLimit);
    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    PowerLimit_k += PID_Calc(&PowerLimit, INA260_1.Power) / PowerLimit.SetPoint;  //用功率环PID调整缩放比？
//    PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k, 1, 0);
    Chassis_Iset = LIMIT_MAX_MIN(Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
//    Chassis_Iset = PowerLimit_k * Chassis_Iset;

    Chassis_CAN1Send(Chassis_Iset);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN1Send(0);
}

int32_t debug_Chassis_Iset;
static void Chassis_DEBUG_PID_Cal(void)
{
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(debug_Chassis_Iset);
}

/**
  * @brief  底盘PID初始化
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
    motor_chassis.pid.P = 20.0f;//0.8f;
	motor_chassis.pid.I = 0;//0.6f;
	motor_chassis.pid.D = 0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
    motor_chassis.pid.OutMax = 5000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
	PowerLimit.SetPoint = 25;  //哨兵底盘功率限制是30W，另外还有200J缓冲热量
	PowerLimit.P = 0.01f;
	PowerLimit.I = 0.01f;
	PowerLimit.D = 0.03f;
	PowerLimit.IMax = 0.5f;
}

void PID_Chassis_ClearError(void)
{  //按理说是可以不用这个函数的，但目前加上一次清零以防万一
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
    PowerLimit.dError = 0.0f;
    PowerLimit.PreError = 0.0f;
    PowerLimit.SumError = 0.0f;
}


/**
 *  @brief  底盘掉电检测
 *  @param  None
 *  @retval None
 */
//void Disconnect_Check(void)
//{
//	int32_t time = GetSysCnt();
//	if(time - block_disconnect.Chassis_Last_Cnt>=10)
//		block_disconnect.Chassis_Sleep_Cnt++;
//	if(block_disconnect.Chassis_Sleep_Cnt>=500)
//		block_disconnect.is_Chassis_Sleep = 1;	
//    
//    /*拨弹的掉电检测也扔到这里以后*/
//}

///**
//  * @brief  返回底盘的结构体指针
//  * @param  None
//  * @retval None
//  */
//_3508_motor_t* Get_MotorChassis(void)
//{
//	return &motor_chassis;
//}
//
///**
//  * @brief  返回setpoint数据的指针
//  * @param  None
//  * @retval None
//  */
//int16_t* GetChassisSetspeed(void)
//{
//	return &ChassisSetSpeed;
//}



