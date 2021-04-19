#include "main.h"
#include "task_ShootUp.h"

_2006_motor_t BodanMotor;
int16_t num, speed;
int16_t Is_Shoot;

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;



void task_ShootUp(void *parameter)
{
    //第一次进任务的复位
    PID_Shoot_Init();

    while (1)
    {
        if (Sentry_State.Shoot_Up_Mode == Shoot_Up_PC)         Shoot_PC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_RC)    Shoot_RC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_SLEEP) Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_DEBUG) Shoot_DEBUG_Act();

        vTaskDelay(1);
    }
}


uint32_t shootDelayTick;          //记录单连发模式下，每两发子弹之间的实时时间间隔
float testInc = 29510.0f;         //26910.0f;//拨一颗弹丸需给到pos的增量值
uint32_t delayTick_oneShot = 200; //走掉一颗弹丸间隔的时间(ms)
float bodanLastPos;               //存放上次单发结束时的拨弹电机位置值

static void Shoot_DEBUG_Act(void)
{
    //FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*设置摩擦轮
    float fsend;

    shootDelayTick++;
    if (shootDelayTick >= delayTick_oneShot)
    {
        bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
        shootDelayTick = 0;                  //清空计数变量值
    }

    BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc;                                   //位置值设定为当前值增加一格弹丸的角度
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //用速度环算电流值输出

    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set); //发拨弹电流
}

//视觉瞄准函数：在激活状态下，如果实际云台姿态和CV给来的云台姿态（即敌人位置）
float pitch_thresh = 6.0f;
float yaw_thresh = 2.0f;
static void aiming(void)
{
    extern PC_Receive_t PC_Receive;
    extern float Pitch_Actual, Yaw_Actual;
    extern uint8_t armor_state; //表示辅瞄是不是有找到目标
    //extern uint8_t CV_is_Catching; //视觉给来一个bit判定是否空闲
    extern uint8_t CV_Shoot_ABLE; //判定视觉方面是否能够打子弹
    //extern uint8_t HEAT_Shoot_ABLE=0;  底盘给来的判定打子弹
    if (armor_state == ARMOR_AIMED &&
        ABS((PC_Receive.RCPitch - Pitch_Actual)) <= pitch_thresh &&
        ABS((PC_Receive.RCYaw - Yaw_Actual)) <= yaw_thresh)
        CV_Shoot_ABLE = 1;
    else CV_Shoot_ABLE = 0;
}
static void Shoot_PC_Act(void)
{
    FrictionWheel_SetSpeed(750, 750); //*设置摩擦轮
    extern uint8_t CV_Shoot_ABLE;  //判定视觉方面是否能够打子弹
    aiming();
    if (CV_Shoot_ABLE)
    {
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
            shootDelayTick = 0;                  //清空计数变量值
        }
        BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
        Shoot_PC_PID_Cal();
    }
    else
        Shoot_SLEEP_PID_Cal();
}

static void Shoot_RC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low,FrictionWheel_R_Speed_Low);//*设置摩擦轮
    //FrictionWheel_SetSpeed(650, 650);

    shootDelayTick++;
    if (shootDelayTick >= delayTick_oneShot)
    {
        bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
        shootDelayTick = 0;                  //清空计数变量值
    }
    BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度

    Shoot_RC_PID_Cal();
}
static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off, FrictionWheel_R_Speed_Off); //*关摩擦轮
    //FrictionWheel_SetSpeed(600,600);
    Shoot_SLEEP_PID_Cal();
}

static void Shoot_PC_PID_Cal(void)
{
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    float fsend;
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0); //用速度环算电流值输出

    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set); //发拨弹电流
}
static void Shoot_RC_PID_Cal(void)
{
    float fsend;

    //纯算pid的过程
    //BodanMotor.pid_pos.SetPoint =  //以后看这里需不需要限幅之类的
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //用速度环算电流值输出

    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, 7500, -7500);
    Bodan_Can2Send(BodanMotor.I_Set); //发拨弹电流
}
inline static void Shoot_SLEEP_PID_Cal(void)
{
    Bodan_Can2Send(0);
}

/**
  * @brief  拨弹电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Shoot_Init(void)
{
    //    BodanMotor.pid_speed.P = 0;
    //    BodanMotor.pid_speed.I = 0.0f;
    //    BodanMotor.pid_speed.D = 0.0f;
    //    BodanMotor.pid_speed.IMax = 500;
    //    BodanMotor.pid_pos.P = 0.0f;
    //    BodanMotor.pid_pos.I = 0.0f;
    //    BodanMotor.pid_pos.D = 0.0f;
    //    BodanMotor.pid_pos.IMax = 500.0f;
    //    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;
    //    BodanMotor.zerocheck.Circle = 0;
    //    BodanMotor.zerocheck.CountCycle = 8191;
    //    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;

    BodanMotor.pid_speed.P = 1.5f;
    BodanMotor.pid_speed.I = 0.0f;
    BodanMotor.pid_speed.D = 0.5f;
    BodanMotor.pid_speed.IMax = 0.0f;

    BodanMotor.pid_pos.P = 0.2f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 10.0f;
    BodanMotor.pid_pos.IMax = 0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}
