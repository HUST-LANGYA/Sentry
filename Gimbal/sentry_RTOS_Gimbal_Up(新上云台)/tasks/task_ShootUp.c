#include "main.h"
#include "task_ShootUp.h"

_2006_motor_t BodanMotor;
_2006_motor_t FrictionMotor[2];

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t Heat_ShootAbleFlag_2 = 0; //底盘给过来的热量控制能否打弹标志位
uint8_t is_game_start = 0;        // 底盘给过来的判断是否正式开始比赛的标志位

//用于摩擦轮和拨弹之间延时的
uint8_t ShootUp_ModeUpdate_Flag = 0;                //标志上次模式和当前模式是否一致
uint8_t BodanDelay_OVER = 0;                        //标志拨弹延时是否已经结束
int32_t BodanDelay_Tick = 0;                        //拨弹开启的延时计数
volatile uint8_t ShootUp_LastMode = Shoot_Up_SLEEP; //存放上次进入shoot_task时的状态
#define BodanDelay_Threshold 500                    //存放拨弹延时的上门限

//堵转检测相关
uint8_t block_bullet_flag;     //卡弹标志 ，置位时表示发生了卡弹，需要做拨盘反转处理
int16_t block_bullet_tick = 0; //卡弹计数器，如果卡弹计数超过一定的秒数，则认为卡弹，置位卡弹标志并执行反转

//******************内部函数声明***********************************************//
static void PID_Shoot_Init(void); //初始化bodan电机的PID参数
static void Shoot_RC_Act(void);
static void Shoot_PC_Act(void);
static void Shoot_SLEEP_Act(void);
static void Shoot_DEBUG_Act(void);
static void aiming(void);
static void Shoot_RC_PID_Cal(void);
static void Shoot_PC_PID_Cal(void);
inline static void Shoot_SLEEP_PID_Cal(void);

void task_ShootUp(void *parameter)
{
    //第一次进任务的复位
    PID_Shoot_Init();

    while (1)
    {
        ShootUp_ModeUpdate_Flag = (ShootUp_LastMode != Sentry_State.Shoot_Up_Mode); 
        ShootUp_LastMode = Sentry_State.Shoot_Up_Mode;                              //更新上次状态
        BodanDelay_Tick = (ShootUp_ModeUpdate_Flag) ? (0) : (BodanDelay_Tick + 1);  //根据状态是否切换，判断延时tick是清零还是递增
        BodanDelay_OVER = (BodanDelay_Tick >= BodanDelay_Threshold);                //根据延时时间和门限值，判定延时是否结束

        //先来堵转检测
        if (ABS(BodanMotor.RealSpeed) < 5 && ABS(BodanMotor.I_Set) > 1000)
            block_bullet_tick++; //堵转计数
        else
            block_bullet_tick = (block_bullet_tick > 0) ? (block_bullet_tick - 1) : 0; //堵转计数的单稳清零
        if (block_bullet_tick >= 3000)
            block_bullet_flag = 1; //堵转3s,则认为堵转，标志置位
        if (block_bullet_tick <= 0)
            block_bullet_flag = 0; //只有计数归零之后，才认为不再堵转

				aiming();
        if (Sentry_State.Shoot_Up_Mode == Shoot_Up_PC)
        {
            //if(is_game_start)  //后期需要加上比赛开始的处理
            Shoot_PC_Act();
            //else Shoot_SLEEP_Act();
        }
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_RC)      Shoot_RC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_SLEEP)   Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_DEBUG)   Shoot_DEBUG_Act();

        vTaskDelay(1);
    }
}

//这几个变量是每个shoot_Act共用的，以后也许可以改成一个结构体，然后各自的Act用各自的成员值
uint32_t shootDelayTick;         //记录单连发模式下，每两发子弹之间的实时时间间隔
float testInc = 29510.0f;        //26910.0f;//拨一颗弹丸需给到pos的增量值
uint32_t delayTick_oneShot = 50; //走掉一颗弹丸间隔的时间(ms)
float bodanLastPos;              //存放上次单发结束时的拨弹电机位置值
float RC_Bodan;

int16_t test_fric_speed0 = 0, test_fric_speed1 = 0;
int8_t Bodan_Enable_DEBUG = DISABLE;
static void Shoot_DEBUG_Act(void)
{
    FrictionWheel_SetSpeed(test_fric_speed0, test_fric_speed1); //*设置摩擦轮
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
    Bodan_Can1Send(BodanMotor.I_Set * Bodan_Enable_DEBUG); //发拨弹电流
}

//视觉瞄准函数：在激活状态下，如果实际云台姿态和CV给来的云台姿态（即敌人位置）
float pitch_thresh = 2.0f;
float yaw_thresh = 1.0f;
static void aiming(void)
{
    extern PC_Receive_t PC_Receive;
    extern float Pitch_Actual, Yaw_Actual;
    extern uint8_t armor_state;   //表示辅瞄是不是有找到目标
    extern uint8_t CV_Shoot_ABLE; //判定视觉方面是否能够打子弹

    if (armor_state == ARMOR_AIMED &&
        ABS((PC_Receive.RCPitch - Pitch_Actual)) <= pitch_thresh &&
        ABS((PC_Receive.RCYaw - Yaw_Actual)) <= yaw_thresh)
		{
        CV_Shoot_ABLE = 1;
			  //armor_state = ARMOR_NO_AIM; //清除瞄准的标志位
		}
    else CV_Shoot_ABLE = 0;
}
static void Shoot_PC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_High, FrictionWheel_R_Speed_High); //*设置摩擦轮
    extern uint8_t CV_Shoot_ABLE;     //判定视觉方面是否能够打子弹
    //aiming();
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
uint8_t Positive_Flag=1;
static void Shoot_RC_Act(void)
{
    //FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*设置摩擦轮
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_High, FrictionWheel_L_Speed_High);
//    Heat_ShootAbleFlag_2 = 1;
//    if (Heat_ShootAbleFlag_2)
//    {
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //有减速比啊啊啊啊！___*( ￣皿￣)/#____
            shootDelayTick = 0;                  //清空计数变量值
        }
      if (!block_bullet_flag)                                   //正常拨弹
            BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //位置值设定为当前值增加一格弹丸的角度
      else 
				    BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc / 10.0f);
			 // {				//堵转处理
//					  if(Positive_Flag)
//            BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc / 10.0f);
//						else
//						BodanMotor.pid_pos.SetPoint = bodanLastPos + (testInc / 10.0f);
//						
//					  Positive_Flag = !Positive_Flag;
//				}
				//BodanMotor.pid_pos.SetPoint = RC_Bodan;
        Shoot_RC_PID_Cal();
//    }
//    else
//        Shoot_SLEEP_PID_Cal();
}
static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off, FrictionWheel_R_Speed_Off); //*关摩擦轮
    Shoot_SLEEP_PID_Cal();
}

static void Shoot_PC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0); //用速度环算电流值输出
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
//    Bodan_Can1Send(BodanDelay_OVER ? BodanMotor.I_Set : 0); //发拨弹电流
	  BodanMotor.I_Set = BodanDelay_OVER ?(BodanMotor.I_Set) : (0);
	Bodan_Can1Send(BodanMotor.I_Set); //发拨弹电流
}
static void Shoot_RC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //用位置环算速度环设定值
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //用速度环算电流值输出
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, 10000, -10000);
	  BodanMotor.I_Set = BodanDelay_OVER ?(BodanMotor.I_Set) : (0);
	  Bodan_Can1Send(BodanMotor.I_Set); //发拨弹电流
}
inline static void Shoot_SLEEP_PID_Cal(void)
{
	 BodanMotor.I_Set = 0;
    Bodan_Can1Send(0);
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
    FrictionMotor[0].pid_speed.P = 25.0f;//27.0f;
    FrictionMotor[0].pid_speed.I = 0.2f;//1.2f;//0.0f;
    FrictionMotor[0].pid_speed.D = 0.0f;
    FrictionMotor[0].pid_speed.IMax = 800.0f;//100;
    FrictionMotor[0].pid_speed.SetPoint = 0;
    //    FrictionMotor[0].pid_pos.P = 0.15f;
    //    FrictionMotor[0].pid_pos.I = 0.0f;
    //    FrictionMotor[0].pid_pos.D = 22.0f;
    //    FrictionMotor[0].pid_pos.IMax = 0.0f;
    //    FrictionMotor[0].pid_pos.SetPoint = BodanMotor.Angle_ABS;
    FrictionMotor[0].zerocheck.Circle = 0;
    FrictionMotor[0].zerocheck.CountCycle = 8191;
    FrictionMotor[0].zerocheck.LastValue = FrictionMotor[0].Angle_ABS;

    FrictionMotor[1].pid_speed.P = 25.0f;//27.0f;
    FrictionMotor[1].pid_speed.I = 0.2f;//0.0f;
    FrictionMotor[1].pid_speed.D = 0.0f;
    FrictionMotor[1].pid_speed.IMax = 800.0f;//100;//0.0f;
    FrictionMotor[1].pid_speed.SetPoint = 0;
    //    FrictionMotor[1].pid_pos.P = 0.15f;
    //    FrictionMotor[1].pid_pos.I = 0.0f;
    //    FrictionMotor[1].pid_pos.D = 22.0f;
    //    FrictionMotor[1].pid_pos.IMax = 0.0f;
    //    FrictionMotor[1].pid_pos.SetPoint = BodanMotor.Angle_ABS;
    FrictionMotor[1].zerocheck.Circle = 0;
    FrictionMotor[1].zerocheck.CountCycle = 8191;
    FrictionMotor[1].zerocheck.LastValue = FrictionMotor[1].Angle_ABS; 
	
	
	
	
    BodanMotor.pid_speed.P = 1.8f;//1.5f;
    BodanMotor.pid_speed.I = 0.0f;
    BodanMotor.pid_speed.D = 0.0f;//0.5f;
    BodanMotor.pid_speed.IMax = 0.0f;

    BodanMotor.pid_pos.P = 0.28f;//0.28f;
    BodanMotor.pid_pos.I = 0.08f;//0.0f;
    BodanMotor.pid_pos.D = 0.0f;//10.0f;
    BodanMotor.pid_pos.IMax = 800.0f;//0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}
