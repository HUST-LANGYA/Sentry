#include "main.h"
#include "task_ShootUp.h"

_2006_motor_t BodanMotor;

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t Heat_ShootAbleFlag_0 = 0; //���̸����������������ܷ�򵯱�־λ
uint8_t is_game_start = 0;        // ���̸��������ж��Ƿ���ʽ��ʼ�����ı�־λ

//����Ħ���ֺͲ���֮����ʱ��
uint8_t ShootUp_ModeUpdate_Flag = 0;                //��־�ϴ�ģʽ�͵�ǰģʽ�Ƿ�һ��
uint8_t BodanDelay_OVER = 0;                        //��־������ʱ�Ƿ��Ѿ�����
int32_t BodanDelay_Tick = 0;                        //������������ʱ����
volatile uint8_t ShootUp_LastMode = Shoot_Up_SLEEP; //����ϴν���shoot_taskʱ��״̬
#define BodanDelay_Threshold 500                    //��Ų�����ʱ��������

//��ת������
uint8_t block_bullet_flag;     //������־ ����λʱ��ʾ�����˿�������Ҫ�����̷�ת����
int16_t block_bullet_tick = 0; //���������������������������һ��������������Ϊ��������λ������־��ִ�з�ת

//******************�ڲ���������***********************************************//
static void PID_Shoot_Init(void); //��ʼ��bodan�����PID����
static void Shoot_RC_Act(void);
static void Shoot_PC_Act(void);
static void Shoot_SLEEP_Act(void);
static void Shoot_DEBUG_Act(void);

static void Shoot_RC_PID_Cal(void);
static void Shoot_PC_PID_Cal(void);
inline static void Shoot_SLEEP_PID_Cal(void);

void task_ShootUp(void *parameter)
{
    //��һ�ν�����ĸ�λ
    PID_Shoot_Init();

    while (1)
    {
        ShootUp_ModeUpdate_Flag = (ShootUp_LastMode != Sentry_State.Shoot_Up_Mode); 
        ShootUp_LastMode = Sentry_State.Shoot_Up_Mode;                              //�����ϴ�״̬
        BodanDelay_Tick = (ShootUp_ModeUpdate_Flag) ? (0) : (BodanDelay_Tick + 1);  //����״̬�Ƿ��л����ж���ʱtick�����㻹�ǵ���
        BodanDelay_OVER = (BodanDelay_Tick >= BodanDelay_Threshold);                //������ʱʱ�������ֵ���ж���ʱ�Ƿ����

        //������ת���
        if (ABS(BodanMotor.RealSpeed) < 5 && ABS(BodanMotor.I_Set) > 1000)
            block_bullet_tick++; //��ת����
        else
            block_bullet_tick = (block_bullet_tick > 0) ? (block_bullet_tick - 1) : 0; //��ת�����ĵ�������
        if (block_bullet_tick >= 1000)
            block_bullet_flag = 1; //��ת3s,����Ϊ��ת����־��λ
        if (block_bullet_tick <= 0)
            block_bullet_flag = 0; //ֻ�м�������֮�󣬲���Ϊ���ٶ�ת

        if (Sentry_State.Shoot_Up_Mode == Shoot_Up_PC)
        {
            //if(is_game_start)
            Shoot_PC_Act();
            //else Shoot_SLEEP_Act();
        }
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_RC)      Shoot_RC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_SLEEP)   Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_DEBUG)   Shoot_DEBUG_Act();

        vTaskDelay(1);
    }
}

//�⼸��������ÿ��shoot_Act���õģ��Ժ�Ҳ�����Ըĳ�һ���ṹ�壬Ȼ����Ե�Act�ø��Եĳ�Աֵ
uint32_t shootDelayTick;         //��¼������ģʽ�£�ÿ�����ӵ�֮���ʵʱʱ����
float testInc = 29510.0f;        //26910.0f;//��һ�ŵ��������pos������ֵ
uint32_t delayTick_oneShot = 50; //�ߵ�һ�ŵ�������ʱ��(ms)
float bodanLastPos;              //����ϴε�������ʱ�Ĳ������λ��ֵ
float debug_setpoint = 0;
int16_t test_fric_speed0 = 0, test_fric_speed1 = 0;
int8_t Bodan_Enable_DEBUG = DISABLE;
static void Shoot_DEBUG_Act(void)
{
    //FrictionWheel_SetSpeed(test_fric_speed0, test_fric_speed1); //*����Ħ����
    float fsend;

    shootDelayTick++;
    if (shootDelayTick >= delayTick_oneShot)
    {
        bodanLastPos = BodanMotor.Angle_Inc; //�м��ٱȰ���������___*( �����)/#____
        shootDelayTick = 0;                  //��ռ�������ֵ
    }

    //BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc;                                   //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
		BodanMotor.pid_pos.SetPoint = debug_setpoint;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //��λ�û����ٶȻ��趨ֵ
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //���ٶȻ������ֵ���

    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set * Bodan_Enable_DEBUG); //����������
}

//�Ӿ���׼�������ڼ���״̬�£����ʵ����̨��̬��CV��������̨��̬��������λ�ã�
float pitch_thresh = 2.0f;
float yaw_thresh = 1.0f;
static void aiming(void)
{
    extern PC_Receive_t PC_Receive;
    extern float Pitch_Actual, Yaw_Actual;
    extern uint8_t armor_state;   //��ʾ�����ǲ������ҵ�Ŀ��
    extern uint8_t CV_Shoot_ABLE; //�ж��Ӿ������Ƿ��ܹ����ӵ�

    if (armor_state == ARMOR_AIMED &&
        ABS((PC_Receive.RCPitch - Pitch_Actual)) <= pitch_thresh &&
        ABS((PC_Receive.RCYaw - Yaw_Actual)) <= yaw_thresh)
        CV_Shoot_ABLE = 1;
    else CV_Shoot_ABLE = 0;
}
static void Shoot_PC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*����Ħ����
    extern uint8_t CV_Shoot_ABLE;     //�ж��Ӿ������Ƿ��ܹ����ӵ�
    aiming();
    if (CV_Shoot_ABLE && Heat_ShootAbleFlag_0)
    {
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //�м��ٱȰ���������___*( �����)/#____
            shootDelayTick = 0;                  //��ռ�������ֵ
        }
        //if(!block_bullet_flag)
        BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
//        else
//            BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc/10.0f);
        Shoot_PC_PID_Cal();
    }
    else
        Shoot_SLEEP_PID_Cal();
}

static void Shoot_RC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low, FrictionWheel_R_Speed_Low); //*����Ħ����
    //FrictionWheel_SetSpeed(600, 600);

//    if (Heat_ShootAbleFlag_0)
//    {
        shootDelayTick++;
        if (shootDelayTick >= delayTick_oneShot)
        {
            bodanLastPos = BodanMotor.Angle_Inc; //�м��ٱȰ���������___*( �����)/#____
            shootDelayTick = 0;                  //��ռ�������ֵ
        }
        if (!block_bullet_flag)                                   //��������
            BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc; //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
        else                                                      //��ת����
            BodanMotor.pid_pos.SetPoint = bodanLastPos - (testInc / 10.0f);
        Shoot_RC_PID_Cal();
//    }
//    else
//        Shoot_SLEEP_PID_Cal();
}
static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off, FrictionWheel_R_Speed_Off); //*��Ħ����
    Shoot_SLEEP_PID_Cal();
}

static void Shoot_PC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //��λ�û����ٶȻ��趨ֵ
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0); //���ٶȻ������ֵ���
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, BodanCurrentLimit, -BodanCurrentLimit);
    Bodan_Can2Send(BodanDelay_OVER ? BodanMotor.I_Set : 0); //����������
}
static void Shoot_RC_PID_Cal(void)
{
    float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc, 0); //��λ�û����ٶȻ��趨ֵ
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed, 0);                       //���ٶȻ������ֵ���
    BodanMotor.I_Set = (int16_t)LIMIT_MAX_MIN(fsend, 7500, -7500);
    Bodan_Can2Send(BodanDelay_OVER ? BodanMotor.I_Set : 0); //����������
}
inline static void Shoot_SLEEP_PID_Cal(void)
{
    Bodan_Can2Send(0);
}

/**
  * @brief  �������pid��ʼ��
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

    BodanMotor.pid_pos.P = 0.23f;//0.2f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 10.0f;
    BodanMotor.pid_pos.IMax = 0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}