#include "main.h"
#include "task_GimbalUp.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.00005f, RCYaw_Scal = 0.00004f; //遥控器对两轴控制的放大比率

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch限幅度参数
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw限幅度参数

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;
extern uint8_t aim_flag;
extern uint8_t defend_flag;
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
static void Gimbal_DEBUG_PID_Cal(void);
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
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos;       /*+ PITCH_GYRO_OFFSET*/; //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;	//   MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc/*+ PITCH_GYRO_OFFSET*/;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;                                                          //   MotoYaw.PidSpeed.SetPoint = Test_Yaw_Veloc;
	
    Gimbal_DEBUG_PID_Cal();
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

    if (GimbalUp_ModeUpdate_Flag) //应该是用来防止模式切换的时候运动抽风的
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
float patrol_step_pitch = 0.005f/*0.035f */, patrol_step_yaw =0.005f;// 0.005f;
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

//		if(defend_flag == 1)
//		{ 
//			//前哨站被打爆后360°
//			PATROL_YAW_MAX_ANGLE = 85;
//			PATROL_YAW_MIN_ANGLE = -85;
//				
//		}
//		
    if (GimbalUp_ModeUpdate_Flag)
    {
        MotoPitch.PidPosV.SetPoint = Pitch_Actual;
        MotoYaw.PidPosV.SetPoint = Yaw_Actual;

        patrol_set_yaw = Yaw_Actual;
        patrol_set_pitch = Pitch_Actual;
    }
    else
    {
        extern  uint8_t armor_state;
        extern  short tx2_last_receive_flag; //表示有没有数据更新
        extern  short tx2_receive_flag;
			  extern  uint16_t Pc_RX_Num;
        extern  uint16_t Last_Pc_Rx_Num;
			  extern Down_Receive_t Down_Receive_Now;
        if (armor_state == ARMOR_AIMED)
        { //若相机捕捉到装甲，则进入瞄准模式，云台姿态根据CV消息来调整
            extern float aim_yaw, aim_pitch;

            MotoPitch.PidPosV.SetPoint = aim_pitch; 
            MotoYaw.PidPosV.SetPoint   = aim_yaw;     
					 //MotoYaw.PidPosV.SetPoint =0;
            patrol_set_pitch = LIMIT_MAX_MIN(aim_pitch, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
            patrol_set_yaw = LIMIT_MAX_MIN(aim_yaw, PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE);
        }
				else if((armor_state == ARMOR_NO_AIM) && (Down_Receive_Now.Valid_flag==1))
				{
				   MotoPitch.PidPosV.SetPoint = Down_Receive_Now.Up_Trans_pitch;
					 MotoYaw.PidPosV.SetPoint = Down_Receive_Now.Up_Trans_yaw;
					 
				    
				}
        else //if(armor_state == ARMOR_NO_AIM)
        {    //若相机未捕捉到装甲，则进入巡逻模式，云台姿态以一个特定的步长来步进
//					  if(aim_flag)  
//					  {
//						  patrol_step_pitch = 0.050;
//							patrol_step_yaw = 0.020;
//						}
						
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

            //MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
						MotoPitch.PidPosV.SetPoint = -15;
            MotoYaw.PidPosV.SetPoint = patrol_set_yaw;
						//MotoYaw.PidPosV.SetPoint=0;

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

    Gyro_Can1Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

static void Gimbal_PC_PID_Cal(void)
{
    float fsend;
    extern uint8_t armor_state; 
    //PITCH限幅+赋值
//	  if(armor_state==ARMOR_NO_AIM)
//		{
     MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
		//}
    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW赋值
		//if(armor_state==ARMOR_NO_AIM)
		//{
     MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, PATROL_YAW_MAX_ANGLE,  PATROL_YAW_MIN_ANGLE);
		//}
    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, MotoYaw.I_Set);
}


static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can1Send(0, 0);
}


static void Gimbal_DEBUG_PID_Cal(void)
{
     float fsend;
  
    //PITCH限幅+赋值 
	  
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;  //1000.0f是由于陀螺仪的返回的角速度
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, 0);


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
			  //如果要实现前后分开检测的话可能需要设定两个初始化值
        moto_pitch_init = 3371.0f;
        gyro_pitch_init = GyroPitchOutPut();
        //moto_yaw_init=YawAngleOutPut();
        moto_yaw_init = 6822.0f;
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
    PITCH_MAX_ANGLE =+25.0f; //尽量不要打太远，会被对面的高地挡住
    PITCH_ZERO_POS = +0;
    PITCH_MIN_ANGLE =-32.0f;

    YAW_MAX_ANGLE = 80;
    YAW_ZERO_POS = 0;
    YAW_MIN_ANGLE = -80;

    //巡逻的范围一般比限位的范围要窄，因为相机的广角足够宽
    PATROL_PITCH_MAX_ANGLE =+20.0f; //-3.0f;
    PATROL_PITCH_ZERO_POS = +0;
    PATROL_PITCH_MIN_ANGLE =-28.0f; //-30.0f;

    PATROL_YAW_MAX_ANGLE = 60;//364
    PATROL_YAW_ZERO_POS =  0;
    PATROL_YAW_MIN_ANGLE =-60;//-36;
}

/**
  * @brief  云台电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{
    MotoPitch.PidPos.P = -550.0f;//-450.0f;//-500.0f;   //-450.0f
    MotoPitch.PidPos.I = -1.2f;//-1.5f;     //-0.7f
    MotoPitch.PidPos.D = 0.0f;
    MotoPitch.PidPos.IMax = 1200.0f;//1000.0f;

    MotoPitch.PidSpeed.P = -3000.0f;//-2500.0f;//-6500.0f;//-8000.0f; //-2500.0f
    MotoPitch.PidSpeed.I = -10.0f;//-16.0f;//-5.0f;               //-16.0f
    MotoPitch.PidSpeed.D = 0.0f;                
    MotoPitch.PidSpeed.IMax = 500.0f;           //800.0f

//    MotoPitch.PidPosV.P = -300.0f;
//    MotoPitch.PidPosV.I = -0.4f;
//    MotoPitch.PidPosV.D = -2.0f; //1800.0f;
//    MotoPitch.PidPosV.IMax = 1200.0f;
    
    MotoPitch.PidPosV.P = -380.0f;//-300.0f;//-280.0f;//-100.0f;//-200.0f; //-300.0f;
    MotoPitch.PidPosV.I = -4.0f;//-2.8f;//-3.0f;//-1.5f;//-0.25f; //-5.0f;
    MotoPitch.PidPosV.D = 0.0f;//-2.0f; //1800.0f;
    MotoPitch.PidPosV.IMax = 100.0f;//200.0f;//300.0f; //50.0f;

    MotoPitch.PidSpeedV.P = -4500.0f;//-5000.0f;//-6000.0f; //-5000.0f;
    MotoPitch.PidSpeedV.I = -15.0f;//-1.5f;    //15
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 2000.0f;//500.0f; //2000.0f;

    //    MotoPitch.PidPosV.P = 0.0f;/
    //    MotoPitch.PidPosV.I = 0.0f;
    //    MotoPitch.PidPosV.D = 0.0f;
    //    MotoPitch.PidPosV.IMax = 5.0f;

    //    MotoPitch.PidSpeedV.P = 0.0f;
    //    MotoPitch.PidSpeedV.I = 0.0f;
    //    MotoPitch.PidSpeedV.D = 0.0f;
    //    MotoPitch.PidSpeedV.IMax = 50.0f;

    //////////////////////////////////////////////////////////////
    MotoYaw.PidPos.P = 0.36f;//-0.33f;//0.35f;//0.65f; //0.5f//-0.36f;//-0.32f//-0.5f
    MotoYaw.PidPos.I = 0.0000f;//-0.0012f;//0.0008f; //0.0008f//-0.001f //-0.0004f//-0.005
    MotoYaw.PidPos.D = 0.0f;//3.8f;
    MotoYaw.PidPos.I_Limit = 0.5;
    MotoYaw.PidPos.IMax = 250.0f;//200.0f; //250

    MotoYaw.PidSpeed.P = 9500.0f;//-9000.0f;//7000.0f;//8000.0f; //7000.0f;//-0.36f//-9500.0f;
    MotoYaw.PidSpeed.I = 10.0f;//10.0f;//2.0f;    //10.0f;//这里改了个符号
    MotoYaw.PidSpeed.D = 0.0f;    
    MotoYaw.PidSpeed.I_Limit = 1;
    MotoYaw.PidSpeed.IMax = 1000.0f;

    MotoYaw.PidPosV.P = 0.45f;//-0.36f;//0.01f;//0.5f;//0.01f;//-0.05f;
    MotoYaw.PidPosV.I = 0.002f;//-0.0015f;//0.0008f;//-0.005f;
    MotoYaw.PidPosV.D = 0.0f;//2.0f;//3.8f;//2.0f;
    MotoYaw.PidPosV.I_Limit = 0.0f;
    MotoYaw.PidPosV.IMax =100; //5000.0f;//0.0f;//400.0f

    MotoYaw.PidSpeedV.P = 8000.0f; //8000.0f
    MotoYaw.PidSpeedV.I = 2.0f;//-15.0f;//2.0f;
    MotoYaw.PidSpeedV.D = 0.0f;
    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    MotoYaw.PidSpeedV.IMax = 1500.0f;

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
