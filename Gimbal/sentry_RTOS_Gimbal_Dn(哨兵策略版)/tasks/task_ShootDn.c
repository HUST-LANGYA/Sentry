#include "main.h"
#include "task_ShootDn.h"

_2006_motor_t BodanMotor;
_2006_motor_t FrictionMotor[2];

//int16_t num, speed;
//int16_t Is_Shoot;

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t Heat_ShootAbleFlag_2 = 0;  //底盘给过来的热量控制能否打弹标志位
uint8_t is_game_start = 0;// 底盘给过来的判断是否正式开始比赛的标志位

//用于摩擦轮和拨弹之间延时的
uint8_t ShootDn_ModeUpdate_Flag = 0;                //标志上次模式和当前模式是否一致
uint8_t BodanDelay_OVER = 0;                        //标志拨弹延时是否已经结束
int32_t BodanDelay_Tick = 0;                        //拨弹开启的延时计数
volatile uint8_t ShootDn_LastMode = Shoot_Dn_SLEEP; //存放上次进入shoot_task时的状态
#define BodanDelay_Threshold 500                    //存放拨弹延时的上门限

//堵转检测相关
uint8_t block_bullet_flag;     //卡弹标志 ，置位时表示发生了卡弹，需要做拨盘反转处理
int16_t block_bullet_tick = 0; //卡弹计数器，如果卡弹计数超过一定的秒数，则认为卡弹，置位卡弹标志并执行反转



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

    while (1)
    {
        ShootDn_ModeUpdate_Flag = (ShootDn_LastMode != Sentry_State.Shoot_Dn_Mode); //?  0: 1;
        ShootDn_LastMode = Sentry_State.Shoot_Dn_Mode;                              //更新上次状态
        BodanDelay_Tick = (ShootDn_ModeUpdate_Flag) ? (0) : (BodanDelay_Tick + 1);  //根据状态是否切换，判断延时tick是清零还是递增
        BodanDelay_OVER = (BodanDelay_Tick >= BodanDelay_Threshold);                //根据延时时间和门限值，判定延时是否结束

			  //先来堵转检测
        if (ABS(BodanMotor.RealSpeed) < 5 && ABS(BodanMotor.I_Set) > 1000)
            block_bullet_tick++; //堵转计数
        else
            block_bullet_tick = (block_bullet_tick > 0) ? (block_bullet_tick - 1) : 0; //堵转计数的单稳清零
        if (block_bullet_tick >= 2000)
            block_bullet_flag = 1; //堵转2s,则认为堵转，标志置位
        if (block_bullet_tick <= 0)
            block_bullet_flag = 0; //只有计数归零之后，才认为不再堵转

			
			
        if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_SLEEP)         Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_PC)
        {
            //if(is_game_start)
            Shoot_PC_Act();
            //else Shoot_SLEEP_Act();
        }
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_RC)          Shoot_RC_Act();
        else if (Sentry_State.Shoot_Dn_Mode == Shoot_Dn_DEBUG)    Shoot_DEBUG_Act();
        
        vTaskDelay(1);
    }
}

//这几个变量是每个shoot_Act共用的，以后也许可以改成一个结构体，然后各自的Act用各自的成员值
uint32_t shootDelayTick;           //记录单连发模式下，每两发子弹之间的实时时间间隔
float testInc = 29510.0f;          //拨弹丸需给到pos的增量值 (这个值比较难调到刚好拨走一颗 8192*36/10 = 29491)
uint32_t delayTick_oneShot = 2000; //走掉一颗弹丸间隔的时间(ms)（一般调试的时候设在1000ms~2000ms之间可能比较合适）
float bodanLastPos;                //存放上次单发结束时的拨弹电机位置值

int16_t test_fric_speed0, test_fric_speed1;
int8_t Bodan_Enable_DEBUG = DISABLE;
static void Shoot_DEBUG_Act(void)
{
//    FrictionWheel_SetSpeed(test_fric_speed0, test_fric_speed1); //*设置摩擦轮
//    float fsend;

//    //计算给pid的设定值的过程
//    shootDelayTick++;
//    if (shootDelayTick >= delayTick_oneShot)
//    {
//        bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
//        shootDelayTick = 0;                  //清空计数变量值
//    }

//    //纯算pid的过程
//    BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc;                                   //位置值设定为当前值增加一格弹丸的角度
//    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
//    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //用速度环算电流值输出

//    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
//    Bodan_Can2Send(/*BodanDelay_OVER?*/ BodanMotor.I_Set * Bodan_Enable_DEBUG /*:0*/); //发拨弹电流
}

//视觉瞄准函数：在激活状态下，如果实际云台姿态和CV给来的云台姿态（即敌人位置）
float pitch_thresh = 2.0f;
float yaw_thresh = 2.0f;
static void aiming(void)
{
    extern PC_Receive_t PC_Receive;
    extern float Pitch_Actual, Yaw_Actual;
    extern uint8_t armor_state; //表示辅瞄是不是有找到目标
    extern uint8_t CV_Shoot_ABLE; //判定视觉方面是否能够打子弹
    
    if (armor_state == ARMOR_AIMED &&
        ABS((PC_Receive.RCPitch - Pitch_Actual)) <= pitch_thresh &&
        ABS((PC_Receive.RCYaw - Yaw_Actual)) <= yaw_thresh)
        CV_Shoot_ABLE = 1;
    else CV_Shoot_ABLE = 0;
}
static void Shoot_PC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*设置摩擦轮
    extern uint8_t CV_Shoot_ABLE;                                                 //判定视觉方面是否能够打子弹
    aiming();
    if (CV_Shoot_ABLE /*&& Heat_ShootAbleFlag_2*/)
    {
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
            shootDelayTick = 0;                  //清空计数变量值
        }
         if(!block_bullet_flag)
        BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
        else
            BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc/10.0f);
				
        Shoot_PC_PID_Cal();
    }
    else
        Shoot_SLEEP_PID_Cal();
}

int16_t RC_test_fric_speed0 = /*5000*/FrictionWheel_L_Speed_Low, RC_test_fric_speed1 = /*5000*/FrictionWheel_R_Speed_Low;
static void Shoot_RC_Act(void)
{
    FrictionWheel_SetSpeed(RC_test_fric_speed0, RC_test_fric_speed1); //*设置摩擦轮

//    if(Heat_ShootAbleFlag_2)
//    {
        //计算给pid的设定值的过程
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
            shootDelayTick = 0;                  //清空计数变量值
        }
				
         if(!block_bullet_flag)
        BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
        else
            BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc/10.0f);
        Shoot_RC_PID_Cal();
//    }
//    else  Shoot_SLEEP_PID_Cal();
}

static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off,FrictionWheel_R_Speed_Off); //*关摩擦轮
    Shoot_SLEEP_PID_Cal();
}

static void Shoot_PC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0); //用速度环算电流值输出
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send((BodanDelay_OVER) ? BodanMotor.I_Set : 0); //发拨弹电流
}

static void Shoot_RC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //用速度环算电流值输出
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send(BodanDelay_OVER ? BodanMotor.I_Set : 0); //发拨弹电流
}

inline static void Shoot_SLEEP_PID_Cal(void)
{
	 BodanMotor.I_Set = 0;
    Bodan_Can2Send(0);
}

/**
  * @brief  拨弹电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Shoot_Init(void)
{
    BodanMotor.pid_speed.P = 1.6f;
    BodanMotor.pid_speed.I = 0.0f;
    BodanMotor.pid_speed.D = 0.0f;
    BodanMotor.pid_speed.IMax = 0;
    BodanMotor.pid_pos.P = 0.25f;//0.15f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 0.0f;//22.0f;
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

    FrictionMotor[0].pid_speed.P = 30.0f;//27.0f;
    FrictionMotor[0].pid_speed.I = 1.2f;//0.0f;
    FrictionMotor[0].pid_speed.D = 0.0f; //0.0f
    FrictionMotor[0].pid_speed.IMax = 100.0f; //0.0f
    FrictionMotor[0].pid_speed.SetPoint = 0;
    //    FrictionMotor[0].pid_pos.P = 0.15f;
    //    FrictionMotor[0].pid_pos.I = 0.0f;
    //    FrictionMotor[0].pid_pos.D = 22.0f;
    //    FrictionMotor[0].pid_pos.IMax = 0.0f;
    //    FrictionMotor[0].pid_pos.SetPoint = BodanMotor.Angle_ABS;
    FrictionMotor[0].zerocheck.Circle = 0;
    FrictionMotor[0].zerocheck.CountCycle = 8191;
    FrictionMotor[0].zerocheck.LastValue = FrictionMotor[0].Angle_ABS;

    FrictionMotor[1].pid_speed.P = 30.0f;//27.0f;
    FrictionMotor[1].pid_speed.I = 1.2f;//0.0f;
    FrictionMotor[1].pid_speed.D = 0.0f;
    FrictionMotor[1].pid_speed.IMax = 100.0f;//0;
    FrictionMotor[1].pid_speed.SetPoint = 0;
    //    FrictionMotor[1].pid_pos.P = 0.15f;
    //    FrictionMotor[1].pid_pos.I = 0.0f;
    //    FrictionMotor[1].pid_pos.D = 22.0f;
    //    FrictionMotor[1].pid_pos.IMax = 0.0f;
    //    FrictionMotor[1].pid_pos.SetPoint = BodanMotor.Angle_ABS;
    FrictionMotor[1].zerocheck.Circle = 0;
    FrictionMotor[1].zerocheck.CountCycle = 8191;
    FrictionMotor[1].zerocheck.LastValue = FrictionMotor[1].Angle_ABS;
}
