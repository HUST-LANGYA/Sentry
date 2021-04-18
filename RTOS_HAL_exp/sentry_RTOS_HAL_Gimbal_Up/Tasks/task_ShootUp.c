#include "main.h"

_2006_motor_t BodanMotor;
int16_t num, speed;
int16_t Is_Shoot;

extern int16_t Shoot_init_flag;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

//******************�ڲ���������***********************************************//
static void PID_Shoot_Init(void);  //��ʼ��bodan�����PID����
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
        if (Sentry_State.Shoot_Up_Mode == Shoot_Up_PC) Shoot_PC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_RC) Shoot_RC_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_SLEEP)  Shoot_SLEEP_Act();
        else if (Sentry_State.Shoot_Up_Mode == Shoot_Up_DEBUG) Shoot_DEBUG_Act();
        
        vTaskDelay(1);
    }
}

uint32_t shootDelayTick;  //��¼������ģʽ�£�ÿ�����ӵ�֮���ʵʱʱ����
float testInc = 29510.0f;//26910.0f;//��һ�ŵ��������pos������ֵ
uint32_t delayTick_oneShot = 200;//�ߵ�һ�ŵ�������ʱ��(ms)
float bodanLastPos;  //����ϴε�������ʱ�Ĳ������λ��ֵ

static void Shoot_DEBUG_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low,FrictionWheel_R_Speed_Low);//*����Ħ����   
    float fsend;

    shootDelayTick++;
    if(shootDelayTick >= delayTick_oneShot) 
    {
        bodanLastPos = BodanMotor.Angle_Inc;//�м��ٱȰ���������___*( �����)/#____
        shootDelayTick = 0;//��ռ�������ֵ
    }

    BodanMotor.pid_pos.SetPoint =bodanLastPos + testInc; //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//��λ�û����ٶȻ��趨ֵ        
    fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);  //���ٶȻ������ֵ���

    BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,BodanCurrentLimit,-BodanCurrentLimit);
    Bodan_Can2Send(BodanMotor.I_Set);//����������            
    
}

static void Shoot_PC_Act(void)
{
    //FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low,FrictionWheel_R_Speed_Low);//*����Ħ����
    delay_ms(100);//�ȿ�Ħ����
    //!�������������Ҽ���д�õ��������ƺ�����
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off,FrictionWheel_R_Speed_Off);
    Shoot_SLEEP_PID_Cal();
}

static void Shoot_RC_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Low,FrictionWheel_R_Speed_Low);//*����Ħ����
    //delay_ms(100);//�ȿ�Ħ����
    //!�������������Ҽ���д�õ��������ƺ�����
        //�����pid���趨ֵ�Ĺ���
    
    shootDelayTick++;
    if(shootDelayTick >= delayTick_oneShot) 
    {
        bodanLastPos = BodanMotor.Angle_Inc;//�м��ٱȰ���������___*( �����)/#____
        shootDelayTick = 0;//��ռ�������ֵ
    }
    BodanMotor.pid_pos.SetPoint =bodanLastPos + testInc; //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�

    Shoot_RC_PID_Cal();

}
static void Shoot_SLEEP_Act(void)
{
    FrictionWheel_SetSpeed(FrictionWheel_L_Speed_Off,FrictionWheel_R_Speed_Off);//*��Ħ����
    //*�������������Ҽ���д�õ��������ƺ�����
    Shoot_SLEEP_PID_Cal();
}



static void Shoot_PC_PID_Cal(void)
{
    //�����滹û�Ķ�
//   if (Sentry_State.Shoot_Up_Mode == Shoot_Up_PC)
//    {
//        float fsend;

//        shootDelayTick++;
//        if(shootDelayTick >= delayTick_oneShot) //��������Ϊ600msһ����
//        {
//            bodanLastPos = BodanMotor.Angle_Inc;//�м��ٱȰ���������___*( �����)/#____
//            BodanMotor.pid_pos.SetPoint = bodanLastPos + testInc;//λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
//            BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//��λ�û����ٶȻ��趨ֵ
//                       
//            shootDelayTick = 0;//��ռ�������ֵ
//        }
//        else
//        {
//            BodanMotor.pid_pos.SetPoint =bodanLastPos + testInc; //λ��ֵ�趨Ϊ��ǰֵ����һ����ĽǶ�
//            BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//��λ�û����ٶȻ��趨ֵ        
//        } 
//        
//        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);  //���ٶȻ������ֵ���
//        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
//        Bodan_Can2Send(BodanMotor.I_Set);//����������            
//        
//    }
}
static void Shoot_RC_PID_Cal(void)
{
    float fsend;

    //����pid�Ĺ���
    //BodanMotor.pid_pos.SetPoint =  //�Ժ������費��Ҫ�޷�֮���
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);//��λ�û����ٶȻ��趨ֵ        
    fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);  //���ٶȻ������ֵ���
    
    BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
    Bodan_Can2Send(BodanMotor.I_Set);//����������            
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

    BodanMotor.pid_pos.P = 0.2f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 10.0f;
    BodanMotor.pid_pos.IMax = 0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}
