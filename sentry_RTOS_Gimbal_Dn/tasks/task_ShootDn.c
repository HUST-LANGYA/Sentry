#include "main.h"

_2006_motor_t BodanMotor;
int16_t num, speed;
int16_t Is_Shoot;

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

//******************内部函数声明***********************************************//
static void PID_Shoot_Init(void);  //初始化bodan电机的PID参数
static void Shoot_RC_Act(void);
static void Shoot_PC_Act(void);
static void Shoot_SLEEP_Act(void);
static void Shoot_DEBUG_Act(void);

static void Shoot_RC_PID_Cal(void);
static void Shoot_PC_PID_Cal(void);
inline static void Shoot_SLEEP_PID_Cal(void);

void task_ShootDn(void *parameter)
{
    //第一次进任务的复位
    PID_Shoot_Init();
    //	float fsend;

    while (1)
    {
        if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_SLEEP) Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_PC) Shoot_PC_Act();
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_RC) Shoot_RC_Act();
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_DEBUG) Shoot_DEBUG_Act();

        vTaskDelay(1);
    }
}

//这几个变量是每个shoot_Act共用的，以后也许可以改成一个结构体，然后各自的Act用各自的成员值
uint32_t shootDelayTick;  //记录单连发模式下，每两发子弹之间的实时时间间隔
float testInc = 29510.0f;//拨一颗弹丸需给到pos的增量值 (这个值比较难调到刚好拨走一颗 8192*36/10 = 29491)
uint32_t delayTick_oneShot = 200;//走掉一颗弹丸间隔的时间(ms)（一般调试的时候设在1000ms~2000ms之间可能比较合适）
float bodanLastPos;  //存放上次单发结束时的拨弹电机位置值

static void Shoot_DEBUG_Act(void)
{
    FrictionWheel_SetSpeed(855,855);//*设置摩擦轮   
    float fsend;

    //计算给pid的设定值的过程
    shootDelayTick++;
    if(shootDelayTick >= delayTick_oneShot) 
    {
        bodanLastPos = BodanMotor.Angle_Inc;//有减速比啊啊啊啊！___*( ￣皿￣)/#____
        shootDelayTick = 0;//清空计数变量值
    }

    //纯算pid的过程
    BodanMotor.pid_pos.SetPoint =bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//用位置环算速度环设定值        
    fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);  //用速度环算电流值输出

    BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,BodanCurrentLimit,-BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set);//发拨弹电流            
}


//视觉瞄准函数：在激活状态下，如果实际云台姿态和CV给来的云台姿态（即敌人位置）
float   pitch_thresh =  1.0f;
float   yaw_thresh   =  1.0f;
static void aiming(void)
{
    extern PC_Receive_t PC_Receive;
    extern float Pitch_Actual, Yaw_Actual;
    extern uint8_t armor_state;		//表示辅瞄是不是有找到目标
    //extern uint8_t CV_is_Catching; //视觉给来一个bit判定是否空闲
    extern uint8_t CV_Shoot_ABLE; //判定视觉方面是否能够打子弹
    //extern uint8_t HEAT_Shoot_ABLE=0;  底盘给来的判定打子弹
    if(armor_state == ARMOR_AIMED && 
       ABS((PC_Receive.RCPitch-Pitch_Actual)) <= pitch_thresh &&
       ABS((PC_Receive.RCPitch-Yaw_Actual)) <= yaw_thresh  )
       CV_Shoot_ABLE = 1;
}
static void Shoot_PC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*设置摩擦轮
    extern uint8_t CV_Shoot_ABLE;                                                   //判定视觉方面是否能够打子弹
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
    //!算热量（调用我即将写好的热量控制函数）
    
    //计算给pid的设定值的过程
    shootDelayTick++;
    if(shootDelayTick >= delayTick_oneShot) 
    {
        bodanLastPos = BodanMotor.Angle_Inc;//有减速比啊啊啊啊！___*( ￣皿￣)/#____
        shootDelayTick = 0;//清空计数变量值
    }
    BodanMotor.pid_pos.SetPoint =bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
    
    Shoot_RC_PID_Cal();
}
static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off,FrictionWheel_R_Speed_Off);//*关摩擦轮
    //*算热量（调用我即将写好的热量控制函数）
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
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//用位置环算速度环设定值        
    fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);  //用速度环算电流值输出

    BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,BodanCurrentLimit,-BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set);//发拨弹电流     
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
    BodanMotor.pid_speed.P = 1.4f;
    BodanMotor.pid_speed.I = 0.0f;
    BodanMotor.pid_speed.D = 0.0f;
    BodanMotor.pid_speed.IMax = 0;
    BodanMotor.pid_pos.P = 0.15f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 22.0f;
    BodanMotor.pid_pos.IMax = 0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;
    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
    
    
    //  BodanMotor.pid_speed.P = 4;
    //	BodanMotor.pid_speed.I = 0.2f;
    //	BodanMotor.pid_speed.D = 1.5f;
    //	BodanMotor.pid_speed.IMax = 500;
    //  BodanMotor.pid_pos.P = 0.8f; //0.22f;
    //	BodanMotor.pid_pos.I = 0.05f;
    //	BodanMotor.pid_pos.D = 0.0f;
    //	BodanMotor.pid_pos.IMax = 500.0f;
    //	BodanMotor.pid_pos.SetPoint = ShootMotor.Angle_ABS;
    //  BodanMotor.zerocheck.Circle = 0;
    //	BodanMotor.zerocheck.CountCycle = 8191;
    //	BodanMotor.zerocheck.LastValue = ShootMotor.Angle_ABS;
}
