#include "main.h"
#include "task_GimbalUp.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.000025f, RCYaw_Scal = 0.00002f; //ң������������ƵķŴ����

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw�޷��Ȳ���

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;

//�����л�modeʱ��pitch��yaw����λ��
uint8_t GimbalUp_ModeUpdate_Flag = 0;                 //��־�ϴ�ģʽ�͵�ǰģʽ�Ƿ�һ��
volatile uint8_t GimbalUp_LastMode = Gimbal_Up_SLEEP; //����ϴν���Gimbal_Dn_taskʱ��״̬

//******************�ڲ���������***********************************************//
static float CombPitchOutput(void); //��ȡ�˲����pitch�Ƕȵĺ���
static float CombYawOutput(void);   //��ȡ�˲����yaw�Ƕȵĺ���
static void Gimbal_Limit(void);//��̨�Ƕ��޷�����
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
 * @brief ��̨��������
 * @param ��
 * @retval ��
 */
void task_GimbalUp(void *parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Gimbal_Init(); //��һ��ִ����̨������ ����̨��PID�������г�ʼ��
    Gimbal_Limit();    //�޷�У��
    while (1)
    {
        GimbalUp_ModeUpdate_Flag = (GimbalUp_LastMode != Sentry_State.Gimbal_Up_Mode); //?  0: 1;
        GimbalUp_LastMode = Sentry_State.Gimbal_Up_Mode;                               //�����ϴ�״̬

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

    //PITCH�޷�+��ֵ
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  �޷�+��ֵ
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

/**
  * @brief  ң����ģʽ�£���������̨����״̬
  * @param  ң����ֵ
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
    { //���ģʽû�з����仯��������ִ��ң������ֵ�Ĳ���
        MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
        MotoPitch.PidPos.SetPoint += RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024);
    }

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PCģʽ�£���������̨����״̬
  * @param  None
  * @retval None
  */
float patrol_step_pitch = 0.035f, patrol_step_yaw = 0.013f;
uint8_t patrol_dir_yaw = 0, patrol_dir_pitch = 0;                              //��־���ܵ����ǰѲ�ߵķ���0��ʾ--��1��ʾ++
float patrol_set_pitch = 0.0f, patrol_set_yaw = 0.0f;                          //Ѳ���趨ֵ����
int32_t PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE, PATROL_PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE, PATROL_YAW_ZERO_POS;       //yaw�޷��Ȳ���
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
        { //�������׽��װ�ף��������׼ģʽ����̨��̬����CV��Ϣ������
            extern float aim_yaw, aim_pitch;

            MotoPitch.PidPosV.SetPoint = aim_pitch; 
            MotoYaw.PidPosV.SetPoint = aim_yaw;     
            patrol_set_pitch = LIMIT_MAX_MIN(aim_pitch, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
            patrol_set_yaw = LIMIT_MAX_MIN(aim_yaw, PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE);
        }
        else //if(armor_state == ARMOR_NO_AIM)
        {    //�����δ��׽��װ�ף������Ѳ��ģʽ����̨��̬��һ���ض��Ĳ���������
            if ((patrol_set_pitch + patrol_step_pitch >= PATROL_PITCH_MAX_ANGLE) && patrol_dir_pitch)
                patrol_dir_pitch = 0; //��������޷���
            if ((patrol_set_pitch - patrol_step_pitch <= PATROL_PITCH_MIN_ANGLE) && !patrol_dir_pitch)
                patrol_dir_pitch = 1; //��������޷���
            if ((patrol_set_yaw + patrol_step_yaw >= PATROL_YAW_MAX_ANGLE) && patrol_dir_yaw)
                patrol_dir_yaw = 0; //��������޷���
            if ((patrol_set_yaw - patrol_step_yaw <= PATROL_YAW_MIN_ANGLE) && !patrol_dir_yaw)
                patrol_dir_yaw = 1; //��������޷���
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
  * @brief  ����̨PID�������
  * @param  None
  * @retval None
  */
static void Gimbal_RC_PID_Cal(void)
{
    float fsend;

    //PITCH�޷�+��ֵ
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  �޷�+��ֵ
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

static void Gimbal_PC_PID_Cal(void)
{
    float fsend;

    //PITCH�޷�+��ֵ
    MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW��ֵ
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
  * @brief  ����̨��̬������㣨������ֵ�͵��ֵ�Ļ����˲���
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
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //һ�׻����˲�

    moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360.0f;
    gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //һ�׻����˲�
}

/**
  * @brief  ����̨��ת�Ƕ���λ
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
    PITCH_MAX_ANGLE =-3; //������Ҫ��̫Զ���ᱻ����ĸߵص�ס
    PITCH_ZERO_POS = +0;
    PITCH_MIN_ANGLE =-36;

    YAW_MAX_ANGLE = 60;
    YAW_ZERO_POS = 0;
    YAW_MIN_ANGLE = -60;

    //Ѳ�ߵķ�Χһ�����λ�ķ�ΧҪխ����Ϊ����Ĺ���㹻��
    PATROL_PITCH_MAX_ANGLE = -3; 
    PATROL_PITCH_ZERO_POS = +0;
    PATROL_PITCH_MIN_ANGLE = -30;

    PATROL_YAW_MAX_ANGLE = 30;
    PATROL_YAW_ZERO_POS =  0;
    PATROL_YAW_MIN_ANGLE =-30;
}

/**
  * @brief  ��̨���pid��ʼ��
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
    MotoYaw.zerocheck.LastValue = MotoYaw.Angle_ABS; //����Ƕ�

    Gyro_White.zerocheck_yaw.CountCycle = 360;
    Gyro_White.zerocheck_yaw.Circle = 0;
    Gyro_White.zerocheck_yaw.LastValue = Gyro_White.YAW_ABS; //�����ǵĽǶ�

    Gyro_White.zerocheck_pitch.CountCycle = 360;
    Gyro_White.zerocheck_pitch.Circle = 0;
    Gyro_White.zerocheck_pitch.LastValue = Gyro_White.PITCH; //�����ǵĽǶ�
                                                             
    //	MotoPitch.PidPos.SetPoint = MotoPitch.Angle_ABS;
    //	MotoYaw.PidPos.SetPoint = Gyro_White.YAW_ABS;
}

/**
  * @brief  ��ȡ�˲������̨������̬��pitch��yaw��
  * @param  None
  * @retval pitch��yaw�ĽǶ�
  */
static float CombPitchOutput(void)
{
    return comb_pitch;
}
static float CombYawOutput(void)
{
    return comb_yaw;
}
