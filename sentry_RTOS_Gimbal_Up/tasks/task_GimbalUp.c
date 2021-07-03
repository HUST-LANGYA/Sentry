#include "main.h"
#include "task_GimbalUp.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.000025f, RCYaw_Scal = 0.00002f; //遥控器对两轴控制的放大比率

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch限幅度参数
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw限幅度参数

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;

//用于切换mode时，pitch和yaw不复位的
uint8_t GimbalUp_ModeUpdate_Flag = 0;                 //标志上次模式和当前模式是否一致
volatile uint8_t GimbalUp_LastMode = Gimbal_Up_SLEEP; //存放上次进入Gimbal_Dn_task时的状态

//******************内部函数声明***********************************************//
static float CombPitchOutput(void); //获取滤波后的pitch角度的函数
static float CombYawOutput(void);   //获取滤波后的yaw角度的函数
static void Gimbal_Limit(void);//云台角度限幅函数
static void PID_Gimbal_Init(void);
static void Gimbal_GYRO_Cal(void);

static void Gimbal_RC_Act(void);
static void Gimbal_PC_Act(void);
static void Gimbal_SLEEP_Act(void);
static void Gimbal_DEBUG_Act(void);

static void Gimbal_RC_PID_Cal(void);
static void Gimbal_PC_PID_Cal(void);
inline static void Gimbal_SLEEP_PID_Cal(void);

/**
 * @brief 云台任务主体
 * @param 无
 * @retval 无
 */
void task_GimbalUp(void *parameter)
{
    //进入任务时执行一次复位
    PID_Gimbal_Init(); //第一次执行云台任务是 给云台的PID参数进行初始化
    Gimbal_Limit();    //限幅校正
    while (1)
    {
        GimbalUp_ModeUpdate_Flag = (GimbalUp_LastMode != Sentry_State.Gimbal_Up_Mode); //?  0: 1;
        GimbalUp_LastMode = Sentry_State.Gimbal_Up_Mode;                               //更新上次状态

        Gimbal_Limit();
        Gimbal_GYRO_Cal();

        Yaw_Actual = CombYawOutput();
        Pitch_Actual = CombPitchOutput();

        if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC)      Gimbal_RC_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_PC) Gimbal_PC_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_SLEEP) Gimbal_SLEEP_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_DEBUG) Gimbal_DEBUG_Act();
        vTaskDelay(1);
    }
}

float Test_Pitch_Pos = 0, Test_Yaw_Pos = 0;
float Test_Pitch_Veloc = 0, Test_Yaw_Veloc = 0;
static void Gimbal_DEBUG_Act(void)
{
    //RC_Ctl_t RC_Ctl = getRCData();

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos;                             //+ YAW_GYRO_OFFSET;       //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos /*+ PITCH_GYRO_OFFSET*/; //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
                                                                        //   MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc/*+ PITCH_GYRO_OFFSET*/;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;                                                          //   MotoYaw.PidSpeed.SetPoint = Test_Yaw_Veloc;
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

/**
  * @brief  遥控器模式下，更新上云台工作状态
  * @param  遥控器值
  * @retval None
  */
static void Gimbal_RC_Act(void)
{
    extern RC_Ctl_t RC_Ctl;

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0;  
        MotoPitch.PidPos.SetPoint = 0; 
        Gimbal_init_flag = 1;
    }

    if (GimbalUp_ModeUpdate_Flag)
    {
        MotoYaw.PidPos.SetPoint = Yaw_Actual;
        MotoPitch.PidPos.SetPoint = Pitch_Actual;
    }
    else
    { //如果模式没有发生变化，则正常执行遥控器赋值的部分
        MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
        MotoPitch.PidPos.SetPoint += RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024);
    }

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PC模式下，更新上云台工作状态
  * @param  None
  * @retval None
  */
float patrol_step_pitch = 0.035f, patrol_step_yaw = 0.013f;
uint8_t patrol_dir_yaw = 0, patrol_dir_pitch = 0;                              //标志两周电机当前巡逻的方向，0表示--，1表示++
float patrol_set_pitch = 0.0f, patrol_set_yaw = 0.0f;                          //巡逻设定值更新
int32_t PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE, PATROL_PITCH_ZERO_POS; //pitch限幅度参数
int32_t PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE, PATROL_YAW_ZERO_POS;       //yaw限幅度参数
static void Gimbal_PC_Act(void)
{
    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPosV.SetPoint = 0; 
        MotoPitch.PidPosV.SetPoint = 0; 
        patrol_set_pitch = 0;
        patrol_set_yaw = 0;

        Gimbal_init_flag = 1;
    }

    if (GimbalUp_ModeUpdate_Flag)
    {
        MotoPitch.PidPosV.SetPoint = Pitch_Actual;
        MotoYaw.PidPosV.SetPoint = Yaw_Actual;

        patrol_set_yaw = Yaw_Actual;
        patrol_set_pitch = Pitch_Actual;
    }
    else
    {
        extern uint8_t armor_state;

        if (armor_state == ARMOR_AIMED)
        { //若相机捕捉到装甲，则进入瞄准模式，云台姿态根据CV消息来调整
            extern float aim_yaw, aim_pitch;

            MotoPitch.PidPosV.SetPoint = aim_pitch; 
            MotoYaw.PidPosV.SetPoint = aim_yaw;     
            patrol_set_pitch = LIMIT_MAX_MIN(aim_pitch, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
            patrol_set_yaw = LIMIT_MAX_MIN(aim_yaw, PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE);
        }
        else //if(armor_state == ARMOR_NO_AIM)
        {    //若相机未捕捉到装甲，则进入巡逻模式，云台姿态以一个特定的步长来步进
            if ((patrol_set_pitch + patrol_step_pitch >= PATROL_PITCH_MAX_ANGLE) && patrol_dir_pitch)
                patrol_dir_pitch = 0; //电机到上限反向
            if ((patrol_set_pitch - patrol_step_pitch <= PATROL_PITCH_MIN_ANGLE) && !patrol_dir_pitch)
                patrol_dir_pitch = 1; //电机到下限反向
            if ((patrol_set_yaw + patrol_step_yaw >= PATROL_YAW_MAX_ANGLE) && patrol_dir_yaw)
                patrol_dir_yaw = 0; //电机到左限反向
            if ((patrol_set_yaw - patrol_step_yaw <= PATROL_YAW_MIN_ANGLE) && !patrol_dir_yaw)
                patrol_dir_yaw = 1; //电机到右限反向
            patrol_set_pitch += (patrol_dir_pitch == 1) ? (+patrol_step_pitch) : (-patrol_step_pitch);
            patrol_set_yaw += (patrol_dir_yaw == 1) ? (+patrol_step_yaw) : (-patrol_step_yaw);

            MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
            MotoYaw.PidPosV.SetPoint = patrol_set_yaw;

        }
    }
    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    Gimbal_SLEEP_PID_Cal();
}


/**
  * @brief  上云台PID输出计算
  * @param  None
  * @retval None
  */
static void Gimbal_RC_PID_Cal(void)
{
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

static void Gimbal_PC_PID_Cal(void)
{
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW赋值
    MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}
static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can2Send(0, 0);
}

float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0;
int8_t init_comb_flag = 1;
/**
  * @brief  上云台姿态输出计算（陀螺仪值和电机值的互补滤波）
  * @param  None
  * @retval None
  */
static void Gimbal_GYRO_Cal(void)
{
    if (init_comb_flag)
    {
        //moto_pitch_init=PitchAngleOutPut();
        moto_pitch_init = 7400.0f;
        gyro_pitch_init = GyroPitchOutPut();
        //moto_yaw_init=YawAngleOutPut();
        moto_yaw_init = 2090.0f;
        gyro_yaw_init = GyroYawOutPut();
        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192.0f * 360.0f;
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //一阶互补滤波

    moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360.0f;
    gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //一阶互补滤波
}

/**
  * @brief  上云台旋转角度限位
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
    PITCH_MAX_ANGLE =-3; //尽量不要打太远，会被对面的高地挡住
    PITCH_ZERO_POS = +0;
    PITCH_MIN_ANGLE =-36;

    YAW_MAX_ANGLE = 60;
    YAW_ZERO_POS = 0;
    YAW_MIN_ANGLE = -60;

    //巡逻的范围一般比限位的范围要窄，因为相机的广角足够宽
    PATROL_PITCH_MAX_ANGLE = -3; 
    PATROL_PITCH_ZERO_POS = +0;
    PATROL_PITCH_MIN_ANGLE = -30;

    PATROL_YAW_MAX_ANGLE = 30;
    PATROL_YAW_ZERO_POS =  0;
    PATROL_YAW_MIN_ANGLE =-30;
}

/**
  * @brief  云台电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{
    MotoPitch.PidPos.P = -500.0f;
    MotoPitch.PidPos.I = -1.5f;
    MotoPitch.PidPos.D = 0.0f;
    MotoPitch.PidPos.IMax = 1000.0f;

    MotoPitch.PidSpeed.P = -8000.0f;
    MotoPitch.PidSpeed.I = -5.0f;
    MotoPitch.PidSpeed.D = 0.0f;
    MotoPitch.PidSpeed.IMax = 500.0f;

//    MotoPitch.PidPosV.P = -300.0f;
//    MotoPitch.PidPosV.I = -0.4f;
//    MotoPitch.PidPosV.D = -2.0f; //1800.0f;
//    MotoPitch.PidPosV.IMax = 1200.0f;
    
    MotoPitch.PidPosV.P = -200.0f;
    MotoPitch.PidPosV.I = -0.25f;
    MotoPitch.PidPosV.D = -2.0f; //1800.0f;
    MotoPitch.PidPosV.IMax = 600.0f;

    MotoPitch.PidSpeedV.P = -6000.0f; //5000.0f;
    MotoPitch.PidSpeedV.I = -1.5f;    //0
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 500.0f;

    //    MotoPitch.PidPosV.P = 0.0f;/
    //    MotoPitch.PidPosV.I = 0.0f;
    //    MotoPitch.PidPosV.D = 0.0f;
    //    MotoPitch.PidPosV.IMax = 5.0f;

    //    MotoPitch.PidSpeedV.P = 0.0f;
    //    MotoPitch.PidSpeedV.I = 0.0f;
    //    MotoPitch.PidSpeedV.D = 0.0f;
    //    MotoPitch.PidSpeedV.IMax = 50.0f;

    //////////////////////////////////////////////////////////////
    MotoYaw.PidPos.P = 0.65f;
    MotoYaw.PidPos.I = 0.0008f;
    MotoYaw.PidPos.D = 3.8f;
    MotoYaw.PidPos.I_Limit = 0.5;
    MotoYaw.PidPos.IMax = 5000.0f;

    MotoYaw.PidSpeed.P = 8000.0f;
    MotoYaw.PidSpeed.I = 2.0f;
    MotoYaw.PidSpeed.D = 0.0f;
    MotoYaw.PidSpeed.I_Limit = 1;
    MotoYaw.PidSpeed.IMax = 1000.0f;

    MotoYaw.PidPosV.P = 0.5f;
    MotoYaw.PidPosV.I = 0.0008f;
    MotoYaw.PidPosV.D = 3.8f;
    MotoYaw.PidPosV.I_Limit = 0.0f;
    MotoYaw.PidPosV.IMax = 5000.0f;

    MotoYaw.PidSpeedV.P = 8000.0f;
    MotoYaw.PidSpeedV.I = 2.0f;
    MotoYaw.PidSpeedV.D = 0.0f;
    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    MotoYaw.PidSpeedV.IMax = 1000.0f;

    //    MotoYaw.PidPosV.P = 0.0f;
    //    MotoYaw.PidPosV.I = 0.0f;
    //    MotoYaw.PidPosV.D = 0.0f;
    //    MotoYaw.PidPosV.I_Limit = 0.0f;
    //    MotoYaw.PidPosV.IMax = 0.0f;

    //    MotoYaw.PidSpeedV.P = 0.0f;
    //    MotoYaw.PidSpeedV.I = 0.0f;
    //    MotoYaw.PidSpeedV.D = 0.0f;
    //    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    //    MotoYaw.PidSpeedV.IMax = 0.0f;
    //
    ////////////////////////////////////////////////////////////////////////
    MotoPitch.zerocheck.CountCycle = 8191;
    MotoPitch.zerocheck.Circle = 0;
    MotoPitch.zerocheck.LastValue = MotoPitch.Angle_ABS;

    MotoYaw.zerocheck.CountCycle = 8191;
    MotoYaw.zerocheck.Circle = 0;
    MotoYaw.zerocheck.LastValue = MotoYaw.Angle_ABS; //电机角度

    Gyro_White.zerocheck_yaw.CountCycle = 360;
    Gyro_White.zerocheck_yaw.Circle = 0;
    Gyro_White.zerocheck_yaw.LastValue = Gyro_White.YAW_ABS; //陀螺仪的角度

    Gyro_White.zerocheck_pitch.CountCycle = 360;
    Gyro_White.zerocheck_pitch.Circle = 0;
    Gyro_White.zerocheck_pitch.LastValue = Gyro_White.PITCH; //陀螺仪的角度
                                                             
    //	MotoPitch.PidPos.SetPoint = MotoPitch.Angle_ABS;
    //	MotoYaw.PidPos.SetPoint = Gyro_White.YAW_ABS;
}

/**
  * @brief  获取滤波后的云台两轴姿态（pitch和yaw）
  * @param  None
  * @retval pitch和yaw的角度
  */
static float CombPitchOutput(void)
{
    return comb_pitch;
}
static float CombYawOutput(void)
{
    return comb_yaw;
}
