#include "main.h"
#include "task_GimbalDn.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.00005f, RCYaw_Scal = 0.00004f; //遥控器对两轴控制的放大比率

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch限幅度参数
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw限幅度参数

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag; //感觉这个变量可能不需要了（很久以后再改这个，免得出问题）
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;
extern uint8_t defend_flag;
//用于切换mode时，pitch和yaw不复位的
uint8_t GimbalDn_ModeUpdate_Flag = 0;                 //标志上次模式和当前模式是否一致
volatile uint8_t GimbalDn_LastMode = Gimbal_Dn_SLEEP; //存放上次进入Gimbal_Dn_task时的状态

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
void task_GimbalDn(void *parameter)
{
    //进入任务时执行一次复位
    PID_Gimbal_Init(); //第一次执行云台任务是 给云台的PID参数进行初始化
    Gimbal_Limit();    //限幅校正
    while (1)
    {
        GimbalDn_ModeUpdate_Flag = (GimbalDn_LastMode != Sentry_State.Gimbal_Dn_Mode); //?  0: 1;
        GimbalDn_LastMode = Sentry_State.Gimbal_Dn_Mode;                               //更新上次状态

        Gimbal_Limit();
        Gimbal_GYRO_Cal();

        Yaw_Actual = CombYawOutput();
        Pitch_Actual = CombPitchOutput();

        if (Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_SLEEP)   Gimbal_SLEEP_Act();
        else if (Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_PC) Gimbal_PC_Act();
        else if (Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_RC) Gimbal_RC_Act();
        else if (Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_DEBUG) Gimbal_DEBUG_Act();
        vTaskDelay(1);
    }
}

float Test_Pitch_Pos = 0, Test_Yaw_Pos = 0;
float Test_Pitch_Veloc = 0, Test_Yaw_Veloc = 0;
static void Gimbal_DEBUG_Act(void)
{
    RC_Ctl_t RC_Ctl = getRCData();

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0;   //Yaw_Actual;	  //MotoPitch.Angle_Inc;
        MotoPitch.PidPos.SetPoint = 0; //+ PITCH_GYRO_OFFSET; //Pitch_Actual; // Gyro_White.YAW_INC;
        Gimbal_init_flag = 1;
    }

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos;     //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos; //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
                                                //    MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc;
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f; //注：这两个 /1000.0f 跟陀螺仪返回角速度的量纲有关，如果用电机的及速度应该就不需要这个÷了
    //MotoPitch.PidSpeed.SetPoint = 0;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0) / 1000.0f;
    //MotoYaw.PidSpeed.SetPoint = 0;
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, 0);
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

    if (GimbalDn_ModeUpdate_Flag)
    {
        MotoYaw.PidPos.SetPoint = Yaw_Actual;
        MotoPitch.PidPos.SetPoint = Pitch_Actual;
    }
    else
    { //如果模式没有发生变化，则正常执行遥控器赋值的部分
        MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
        MotoPitch.PidPos.SetPoint += RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024); // + PITCH_GYRO_OFFSET;
    }

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PC模式下，更新上云台工作状态
  * @param  None
  * @retval None
  */
float TestV_Pitch = 0, TestV_Yaw = 0;
float patrol_step_pitch = 0.035f, patrol_step_yaw = 0.03f;
uint8_t patrol_dir_yaw = 0, patrol_dir_pitch = 0;                              //标志两轴电机当前巡逻的方向，0表示--，1表示++
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
        patrol_set_yaw = Yaw_Actual;

        Gimbal_init_flag = 1;
    }

		if(defend_flag == 1)
		{ 
			//前哨站被打爆后360°
			PATROL_YAW_MAX_ANGLE = 85;
			PATROL_YAW_MIN_ANGLE = -85;
				
		}
		
		
    if (GimbalDn_ModeUpdate_Flag)
    {
        MotoPitch.PidPosV.SetPoint = Pitch_Actual;
        MotoYaw.PidPosV.SetPoint = Yaw_Actual;

        patrol_set_yaw = Yaw_Actual;
        patrol_set_pitch = Pitch_Actual;
    }
    else
    {
        extern uint8_t armor_state;
        extern Up_Receive_t Up_Receive_Now;
        if (armor_state == ARMOR_AIMED)
        { //若相机捕捉到装甲，则进入瞄准模式，云台姿态根据CV消息来调整
            extern float aim_yaw, aim_pitch;

            MotoPitch.PidPosV.SetPoint = aim_pitch;
            MotoYaw.PidPosV.SetPoint = aim_yaw;

            patrol_set_pitch = LIMIT_MAX_MIN(aim_pitch, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
            patrol_set_yaw = LIMIT_MAX_MIN(aim_yaw, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
        }
				else if((armor_state == ARMOR_NO_AIM) && (Up_Receive_Now.Valid_flag==1)) //自身处于未识别状态且收到了上云台的指示角度
				{
				   MotoPitch.PidPosV.SetPoint = Up_Receive_Now.Down_Trans_pitch;
				   MotoYaw.PidPosV.SetPoint = Up_Receive_Now.Down_Trans_yaw;
				
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
            patrol_set_pitch += (patrol_dir_pitch) ? (+patrol_step_pitch) : (-patrol_step_pitch);
            patrol_set_yaw += (patrol_dir_yaw) ? (+patrol_step_yaw) : (-patrol_step_yaw);

            MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(patrol_set_pitch, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
            MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(patrol_set_yaw, MIN(PATROL_YAW_MAX_ANGLE,YAW_MAX_ANGLE),MAX( PATROL_YAW_MIN_ANGLE,YAW_MIN_ANGLE));
        }
    }
    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    Gimbal_SLEEP_PID_Cal();
}


/**
  * @brief  下云台PID输出计算
  * @param  None
  * @retval None
  */
static void Gimbal_RC_PID_Cal(void)
{
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f; //注：这两个 /1000.0f 跟陀螺仪返回角速度的量纲有关，如果用电机的及速度应该就不需要这个÷了
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY,0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, MotoYaw.I_Set);
}
static void Gimbal_PC_PID_Cal(void)
{
    float fsend;

    //PITCH限幅+赋值
    MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY,0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW赋值
    MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, YAW_MAX_ANGLE,YAW_MIN_ANGLE);
    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set,MotoYaw.I_Set);
}
static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can1Send(0, 0);
}

float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0; 
int8_t init_comb_flag = 1;
/**
  * @brief  下云台姿态输出计算（陀螺仪值和电机值的互补滤波）
  * @param  None
  * @retval None
  */
static void Gimbal_GYRO_Cal(void)
{
    if (init_comb_flag)
    {
        //moto_pitch_init=PitchAngleOutPut();
        moto_pitch_init = 2016.0f;
        gyro_pitch_init = GyroPitchOutPut();

        //moto_yaw_init=MotoYaw.Angle_ABS;//YawAngleOutPut();
        moto_yaw_init = 1383.0f;//YawAngleOutPut();//3800.0f;
        gyro_yaw_init = GyroYawOutPut();

        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192.0f * 360; //用电机编码器读取的pitch角度值
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);                 //通过陀螺仪读取的pitch角度值
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch;      //一阶互补滤波

    //注意：下云台的yaw轴电机可能是反装的，故给到视觉的时候，需要注意正负号！！！
    moto_yaw = -(YawAngleOutPut() - moto_yaw_init) / (8192.0f / 40.0f * 48.0f) * 360; //用电机编码器读取的yaw角度值
    //由于下云台电机和云台之间用张紧轮和传动带传动，两个半径的比为40:48，故在这里把yaw轴刻度做一个缩放
    gyro_yaw = (GyroYawOutPut() - gyro_yaw_init);        //通过陀螺仪读取的yaw角度值
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //一阶互补滤波
}

/**
  * @brief  下云台旋转角度限位
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
    PITCH_MAX_ANGLE = /*PITCH_GYRO_OFFSET + */ +30;
    PITCH_ZERO_POS = /*PITCH_GYRO_OFFSET + */ 0;
    PITCH_MIN_ANGLE = /*PITCH_GYRO_OFFSET*/ -35;

    YAW_MAX_ANGLE = Yaw_Actual + 20;
    YAW_ZERO_POS = Yaw_Actual + 0;
    YAW_MIN_ANGLE = Yaw_Actual - 20;

    //巡逻的范围一般比限位的范围要窄，因为相机的广角足够宽
    PATROL_PITCH_MAX_ANGLE = /*PITCH_GYRO_OFFSET +*/ 30;
    PATROL_PITCH_ZERO_POS = /*PITCH_GYRO_OFFSET */ +0;
    PATROL_PITCH_MIN_ANGLE = /*PITCH_GYRO_OFFSET -*/ -30; //5+Yaw_Actual/3.0f;  //有个小问题，就是yaw绝对值较大的时候，pitch低头45°会打到玻纤板，所以这里用个函数改一下实时限位值

    PATROL_YAW_MAX_ANGLE = /*YAW_GYRO_OFFSET +*/ 65; //Yaw_Actual + 30;
    PATROL_YAW_ZERO_POS = /*YAW_GYRO_OFFSET + */ 0;  //Yaw_Actual + 0;
    PATROL_YAW_MIN_ANGLE = /*YAW_GYRO_OFFSET */ -65; //Yaw_Actual - 30;
}

/**
  * @brief  云台电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{   //Pitch电机的PID需要需要修改为正的,Yaw轴不需要
    MotoPitch.PidPos.P = 270.0f;//-290;
    MotoPitch.PidPos.I = 0.5f;//-1.2f;
    MotoPitch.PidPos.D = 0;
    MotoPitch.PidPos.IMax = 800.0f;

    MotoPitch.PidSpeed.P = 5000.0f;//-6500.0f;//-9000.0f;
    MotoPitch.PidSpeed.I = 8.5f;//-8.0f;
    MotoPitch.PidSpeed.D = 0;
    MotoPitch.PidSpeed.IMax = 500.0f;

//    MotoPitch.PidPosV.P = -290.0f;
//    MotoPitch.PidPosV.I = -1.2f;
//    MotoPitch.PidPosV.D = 0.0f;
//    MotoPitch.PidPosV.IMax = 800.0f;
    
    MotoPitch.PidPosV.P = 350.0f;//190.0f;//200.0f;//-200.0f;
    MotoPitch.PidPosV.I = 3.8f;//1.5f;//-0.6f;
    MotoPitch.PidPosV.D = 0.0f;
    MotoPitch.PidPosV.IMax = 1500.0f;

    MotoPitch.PidSpeedV.P = 5000.0f;//7000.0f;//5000.0f;//-8000.0f;
    MotoPitch.PidSpeedV.I = 15.0f;//5.0f;//-5.0f;
    MotoPitch.PidSpeedV.D = 0;
    MotoPitch.PidSpeedV.IMax = 500.0f;
    
//    MotoPitch.PidPos.P = 20000.0f;
//    MotoPitch.PidPos.I = 20.0f;
//    MotoPitch.PidPos.D = 0.0f;
//    MotoPitch.PidPos.IMax = 2500.0f;

//    MotoPitch.PidSpeed.P = 250.0f;
//    MotoPitch.PidSpeed.I = 0.0f;
//    MotoPitch.PidSpeed.D = 0;
//    MotoPitch.PidSpeed.IMax = 0.0f;


//    MotoPitch.PidPosV.P = 8000.0f;
//    MotoPitch.PidPosV.I = 2.5f;
//    MotoPitch.PidPosV.D = 0.0f;
//    MotoPitch.PidPosV.IMax = 1000.0f;

//    MotoPitch.PidSpeedV.P = 750.0f;
//    MotoPitch.PidSpeedV.I = 0.0f; //-270.0f;
//    MotoPitch.PidSpeedV.D = 0;
//    MotoPitch.PidSpeedV.IMax = 0.0f;

    /////////////////////////////////////////////////////////

    MotoYaw.PidPos.P = 280.0f;//480.0f;
    MotoYaw.PidPos.I = 0.35f;//0.35f;
    MotoYaw.PidPos.D = 0.0f;//925.0f;
    MotoYaw.PidPos.IMax = 500.0f;

    MotoYaw.PidSpeed.P = -8500.0f;//-9000.0f;
    MotoYaw.PidSpeed.I = -20.0f;//-2.0f;//-5.0f;
    MotoYaw.PidSpeed.D = 0.0f;
    MotoYaw.PidSpeed.IMax = 300.0f;

 //没有除以1000所以会和其他模式差得比较大
    MotoYaw.PidPosV.P = 0.4f; //-280.0f;
    MotoYaw.PidPosV.I = 0.005f; //-0.35f;
    MotoYaw.PidPosV.D = 0;//925.0f;
    MotoYaw.PidPosV.IMax = 100.0f;//0;//500.0f;

    MotoYaw.PidSpeedV.P = -8000.0f;
    MotoYaw.PidSpeedV.I = -4.0f;
    MotoYaw.PidSpeedV.D = 0.0f;
    MotoYaw.PidSpeedV.IMax = 300.0f;



//    MotoYaw.PidPos.P = 0.0f;
//    MotoYaw.PidPos.I = 0.0f;
//    MotoYaw.PidPos.D = 0;
//    MotoYaw.PidPos.I_Limit = 0.5f;
//    MotoYaw.PidPos.IMax = 0.0f;

//    MotoYaw.PidSpeed.P = 0.0f;
//    MotoYaw.PidSpeed.I = 0.0f;
//    MotoYaw.PidSpeed.D = 0.0f;
//    MotoYaw.PidSpeed.I_Limit = 0;
//    MotoYaw.PidSpeed.IMax = 0;


//    MotoYaw.PidPos.P = -12000.0f;
//    MotoYaw.PidPos.I = -15.0f;
//    MotoYaw.PidPos.D = 0;
//    MotoYaw.PidPos.I_Limit = 0.0f;
//    MotoYaw.PidPos.IMax = 2500.0f;

//    MotoYaw.PidSpeed.P = 100.0f;
//    MotoYaw.PidSpeed.I = 0.0f;
//    MotoYaw.PidSpeed.D = 0.0f;
//    MotoYaw.PidSpeed.I_Limit = 0;
//    MotoYaw.PidSpeed.IMax = 0;
//    
//    
//    MotoYaw.PidPosV.P = -1000.0f; //-280.0f;
//    MotoYaw.PidPosV.I = -1.0f; //-0.35f;
//    MotoYaw.PidPosV.D = 0;//925.0f;
//    MotoYaw.PidPosV.IMax = 500.0f;//500.0f;

//    MotoYaw.PidSpeedV.P = 25.0f;
//    MotoYaw.PidSpeedV.I = 0.0f;
//    MotoYaw.PidSpeedV.D = 0.0f;
//    MotoYaw.PidSpeedV.IMax = 0.0f;


    /////////////////////////////////////////////////////////////
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
                                                             //
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

