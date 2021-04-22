#include "main.h"
#include "task_GimbalDn.h"

//受陀螺仪的安装方式的影响，pitch轴水平角度可能会有180或0的偏置值
//设置一个宏，在写setpoint等时候需加上该偏置值
#define PITCH_GYRO_OFFSET 0
#define YAW_GYRO_OFFSET 0  //可能也有有用处，比如上电时陀螺仪yaw轴的0值对应的不是正中间

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.08f, RCYaw_Scal = 0.0002f; //遥控器对两轴控制的放大比率

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch限幅度参数
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw限幅度参数

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;   //感觉这个变量可能不需要了（很久以后再改这个，免得出问题）
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;


/**
 * @brief 云台任务主体
 * @param 无
 * @retval 无
 */
void task_GimbalDn(void *parameter)
{
    //进入任务时执行一次复位
    PID_Gimbal_Init(); //第一次执行云台任务是 给云台的PID参数进行初始化
    Gimbal_Limit();//限幅校正
    //加一个读取上电时两轴电机刻度的函数，即Gimbal_GYRO_Cal();里面只执行一次的那一块
    while (1)
    {
        Gimbal_Limit();
        Gimbal_GYRO_Cal();
        
        Yaw_Actual = CombYawOutput();
        Pitch_Actual = CombPitchOutput();
        
        if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_SLEEP) Gimbal_SLEEP_Act();
        else if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_PC) Gimbal_PC_Act();
        else if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_RC) Gimbal_RC_Act();
        else if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_DEBUG) Gimbal_DEBUG_Act();
        //后面可以继续扩展其他工作模式，然后要记得在头文件里面把对应的宏定义加上
        
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

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos  ;  //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos ;  //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
//    MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc;
//    
    
    float fsend;

    //PITCH限幅+赋值
      MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
      MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0)/1000.0f; //注：这两个 /1000.0f 跟陀螺仪返回角速度的量纲有关，如果用电机的及速度应该就不需要这个÷了
    //MotoPitch.PidSpeed.SetPoint = 0;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0)/1000.0f;
    //MotoYaw.PidSpeed.SetPoint = 0;
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
    RC_Ctl_t RC_Ctl = getRCData();

//    Yaw_Actual = CombYawOutput();
//    Pitch_Actual = CombPitchOutput();

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0;   //Yaw_Actual;	  //MotoPitch.Angle_Inc;
        MotoPitch.PidPos.SetPoint = 0;// + PITCH_GYRO_OFFSET; //Pitch_Actual; // Gyro_White.YAW_INC;
        Gimbal_init_flag = 1;
    }


    MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch0 - 1024);
    MotoPitch.PidPos.SetPoint = RCPitch_Scal * (RC_Ctl.rc.ch1 - 1024);// + PITCH_GYRO_OFFSET;

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PC模式下，更新上云台工作状态
  * @param  None
  * @retval None
  */
float TestV_Pitch = 0, TestV_Yaw = 0;
float patrol_step_pitch = 0.09f,patrol_step_yaw = 0.1f;
uint8_t patrol_dir_yaw = 0,patrol_dir_pitch = 0;  //标志两轴电机当前巡逻的方向，0表示--，1表示++
float patrol_set_pitch = 0.0f,patrol_set_yaw = 0.0f;  //巡逻设定值更新
//FilterFIR filterPC;//滤波器变量
//CoeFIR testCOE; // = {  0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                 };// 这个里面的具体数据用matlab测完（具体怎么测去问LZC去）
static void Gimbal_PC_Act(void)
{
    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPosV.SetPoint = 0;     //Yaw_Actual;//MotoPitch.Angle_Inc;
        MotoPitch.PidPosV.SetPoint = 0; //Pitch_Actual;// Gyro_White.YAW_INC;
        patrol_set_pitch = 0;
        patrol_set_yaw = 0;
                                          //		filterPC=FIRInit(testCOE);//初始化滤波参量
                                          //        for(int i=0;i<20;i++)
                                          //        {
                                          //            testCOE.h[i] = 0.05f;
                                          //        }

        Gimbal_init_flag = 1;
    }
    
    extern uint8_t armor_state;
    
    if(armor_state == ARMOR_AIMED)
    {//若相机捕捉到装甲，则进入瞄准模式，云台姿态根据CV消息来调整
        extern float aim_yaw, aim_pitch;
        MotoPitch.PidPosV.SetPoint = aim_pitch;
        patrol_set_pitch = aim_pitch;
        patrol_set_pitch = LIMIT_MAX_MIN(patrol_set_pitch,0,-45);
        MotoYaw.PidPosV.SetPoint = aim_yaw;
        patrol_set_yaw = aim_yaw;
        patrol_set_yaw = LIMIT_MAX_MIN(patrol_set_yaw,45,-45);
    }
    else //if(armor_state == ARMOR_NO_AIM)
    {//若相机未捕捉到装甲，则进入巡逻模式，云台姿态以一个特定的步长来步进
        if((patrol_set_pitch + patrol_step_pitch >= PITCH_MAX_ANGLE) &&  patrol_dir_pitch) patrol_dir_pitch = 0; //电机到上限反向 
        if((patrol_set_pitch - patrol_step_pitch <= PITCH_MIN_ANGLE) && !patrol_dir_pitch) patrol_dir_pitch = 1; //电机到下限反向 
//        if((patrol_set_yaw   + patrol_step_yaw   >=  YAW_MAX_ANGLE ) &&    patrol_dir_yaw) patrol_dir_yaw   = 0; //电机到左限反向 
//        if((patrol_set_yaw   - patrol_step_yaw   <=  YAW_MIN_ANGLE ) &&   !patrol_dir_yaw) patrol_dir_yaw   = 1; //电机到右限反向 
        patrol_set_pitch += (patrol_dir_pitch)?(+patrol_step_pitch):(-patrol_step_pitch);
//        patrol_set_yaw   += ( patrol_dir_yaw )?( +patrol_step_yaw ):( -patrol_step_yaw );
        patrol_set_yaw   += patrol_step_yaw ;
        
        MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
        MotoYaw.PidPosV.SetPoint   = patrol_set_yaw;
//        MotoPitch.PidPosV.SetPoint = 0;
//        MotoYaw.PidPosV.SetPoint = 0;
    }
    
    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    //! 应该还是要设置一下flag之类的东西在这里
    Gimbal_SLEEP_PID_Cal();
}


//float Limit = 6.0f;
int16_t testYawI = 0, testPitchI = 0;

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
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0)/1000.0f; //注：这两个 /1000.0f 跟陀螺仪返回角速度的量纲有关，如果用电机的及速度应该就不需要这个÷了
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  限幅+赋值
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0)/1000.0f;
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
    //MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, YAW_MAX_ANGLE,YAW_MIN_ANGLE);
    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    //if (GetSysCnt() % 3 == 0)
        //Gyro_Can2Send(MotoPitch.I_Set,0);
        Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}
static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can2Send(0,0);
}


float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0; //0.02;
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
        moto_pitch_init = 6100.0f;
        gyro_pitch_init = GyroPitchOutPut();
        moto_yaw_init=YawAngleOutPut();
        //moto_yaw_init = 4900.0f;
        gyro_yaw_init = GyroYawOutPut();
        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192.0f * 360;  //用电机编码器读取的pitch角度值
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);   //通过陀螺仪读取的pitch角度值
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //一阶互补滤波

    
    //注意：下云台的yaw轴电机可能是反装的，故给到视觉的时候，需要注意正负号！！！
    moto_yaw = -(YawAngleOutPut() - moto_yaw_init) / (8192.0f/40.0f*48.0f) * 360;   //用电机编码器读取的yaw角度值
    //由于下云台电机和云台之间用张紧轮和传动带传动，两个半径的比为40:48，故在这里把yaw轴刻度做一个缩放
    gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);     //通过陀螺仪读取的yaw角度值
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //一阶互补滤波
}

/**
  * @brief  下云台旋转角度限位
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
        //注：云台水平时 pitch=180 （也可能是0，和陀螺仪安装的方向有关）
        PITCH_MAX_ANGLE = /*PITCH_GYRO_OFFSET + */+0;
        PITCH_ZERO_POS = /*PITCH_GYRO_OFFSET + */0;
        PITCH_MIN_ANGLE = /*PITCH_GYRO_OFFSET*/ -46;
      
//        //注： 由于YAW轴的setpoint是增量式的，故这个限幅的改动可以改变旋转的速度
//        YAW_MAX_ANGLE = Yaw_Actual + 60;
//        YAW_ZERO_POS = Yaw_Actual + 0;
//        YAW_MIN_ANGLE = Yaw_Actual - 60;

    YAW_MAX_ANGLE = +75;
    YAW_ZERO_POS  = 0;
    YAW_MIN_ANGLE = -75;
}


/**
  * @brief  云台电机pid初始化
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{
	MotoPitch.PidPos.P =-356.0f;
	MotoPitch.PidPos.I =0;
	MotoPitch.PidPos.D =-1245.0f;
	MotoPitch.PidPos.IMax = 10.0f;

	MotoPitch.PidSpeed.P =-5400.0f;
	MotoPitch.PidSpeed.I =0.0f;//-270.0f;
	MotoPitch.PidSpeed.D =0;
	MotoPitch.PidSpeed.IMax = 50.0f;

//    MotoPitch.PidPos.P = -60.0f;
//    MotoPitch.PidPos.I = -2.0f;
//    MotoPitch.PidPos.D = -3600.0f;
//    MotoPitch.PidPos.IMax = 20.0f;

//    MotoPitch.PidSpeed.P = -3600.0f;
//    MotoPitch.PidSpeed.I = -100.0f;
//    MotoPitch.PidSpeed.D = 0;
//    MotoPitch.PidSpeed.IMax = 50.0f;

    MotoPitch.PidPosV.P = -80.0f;//-330.0f;
    MotoPitch.PidPosV.I = -35.0f;
    MotoPitch.PidPosV.D = 0.0f;//-1245.0f;
    MotoPitch.PidPosV.IMax = 0;//10.0f;

    MotoPitch.PidSpeedV.P = -5000.0f;
    MotoPitch.PidSpeedV.I = -180.0f;
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 50.0f;



/////////////////////////////////////////////////////////

	MotoYaw.PidPos.P = -300.0f;
	MotoYaw.PidPos.I =-10.0f;
	MotoYaw.PidPos.D = +900.0f;
	MotoYaw.PidPos.I_Limit = 0.5;
	MotoYaw.PidPos.IMax = 10.0f;

	MotoYaw.PidSpeed.P =8000.0f;
	MotoYaw.PidSpeed.I =0;
	MotoYaw.PidSpeed.D = 1000.0f;
	MotoYaw.PidSpeed.I_Limit = 1;
	MotoYaw.PidSpeed.IMax = 50;

//    MotoYaw.PidPos.P = 100.0f;
//    MotoYaw.PidPos.I = 1.0f;
//    MotoYaw.PidPos.D = 1.0f;
//    MotoYaw.PidPos.I_Limit = 0.5f;
//    MotoYaw.PidPos.IMax = 10.0f;

//    MotoYaw.PidSpeed.P = 4000.0f;
//    MotoYaw.PidSpeed.I = 1.0f;
//    MotoYaw.PidSpeed.D = 600.0f;
//    MotoYaw.PidSpeed.I_Limit = 1;
//    MotoYaw.PidSpeed.IMax = 500;


    MotoYaw.PidPosV.P = -0.5f;//-0.1f;
    MotoYaw.PidPosV.I = 0.0f;//-50.0f;
    MotoYaw.PidPosV.D = 0.0f;//900.0f;//1.0f;
    MotoYaw.PidPosV.I_Limit =0;// 0.5f;
    MotoYaw.PidPosV.IMax = 0;//10.0f;

    MotoYaw.PidSpeedV.P = 3000.0f;
    MotoYaw.PidSpeedV.I = 0.0f;
    MotoYaw.PidSpeedV.D = 0.0f;//5.0f;
    MotoYaw.PidSpeedV.I_Limit = 0;//1.0f;
    MotoYaw.PidSpeedV.IMax = 0;//5.0f;




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
