#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

//��ʼ����صı�־λ
uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

//�ܻ��������
extern uint16_t last_remainHP,cur_remainHP;
extern uint16_t is_Under_Attack; //����ܻ����򽫸ñ�������ĳ������̬��ֵ


//���ʲ���
float maxPower = 30.0f;
float PowerNow = 0.0f;
float PowerLimit_k=0.8f;
uint16_t remainEnergy;


//�˶�״̬����
MovingDir_t movingDir;//��ǰ�н��ķ���  1Ϊ���� 0Ϊ����  
uint16_t waitingTick;//�ڶ�ͷ��ͣ��ʱ��
uint32_t movingTick;//�˶��м�����
int16_t last_encoder_abs,cur_encoder_abs;

//λ���ں���ر���
extern LimitSWState_t LimitSw_State;//�г̿���״̬
extern PhotoEleSWState_t PhotoEle_State;  //��翪��״̬
extern PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//��������������ĸ���
extern _encoder_t encoder;
int32_t encoder_distance = 0;//�ñ�������¼�����ٶ�֮ǰ��������
int32_t encoder_round = 0; //��¼������ת����Ȧ��
    
//�ܻ���������� ���ⲿ���ӣ�
uint8_t set_escape_period = 4;  //�ܻ����Ѳ��������
uint8_t cur_escape_period = 0; //��ǰѲ�ߵ������� ��һ���ܻ���������������㣬Ȼ�����cur��set�в��һֱѲ��
//���Ҳ��翪�ش���������


static void Chassis_Patrol_Act(void);
static void Chassis_RC_Act(void);
static void Chassis_SLEEP_Act(void);
static void Chassis_DEBUG_Act(void);
static void Chassis_Patrol_PID_Cal(void);
static void Chassis_RC_PID_Cal(void);
inline static void Chassis_SLEEP_PID_Cal(void);
static void Chassis_DEBUG_PID_Cal(void);
static void PID_Chassis_Init(void);

void task_Chassis(void* parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Chassis_Init(); //��һ��ִ�е�������ʱ �����̵�PID�������г�ʼ��
    
    //������������ʼ������
    TIM_SetCounter(TIM3,22463);
    encoder.angle_abs = 22463;//��װ�ϵ�ʱ���ڹ���Ŀ��ұ�
	encoder.zerocheck.CountCycle = 65535; //��Ȼ�����ʼ���˱�����������Ľṹ�壬����ʵ�⣬2021�����Ĺ���������int16_t����
	encoder.zerocheck.LastValue = encoder.angle_abs;
	encoder.zerocheck.Circle = 0;
    
    //�˶����Գ�ʼ������
    cur_escape_period = set_escape_period; //�趨��ʼʱ��������ܣ�����ֹ����
    
    while (1)
    {
        //ǹ���������ƵĲ���
        //TODO! ���԰������������Ĳ��ֵַ�һ��������������
        HeatUpdate_0();   //����ǹ������ֵ��������������ֵ����ͨ��CAN���͵�������̨����ȥ
        HeatUpdate_2();   //17mmǹ�ܵ�ID�Ű�ż��������
        ShootingHeat_CAN2Send();    //���͸�������̨����������
        
        //��������λ���ں����
        encoder.angle_abs = Read_Encoder();   // ��TIM��CNT�Ĵ������³��µļƲ�ֵ
        last_encoder_abs = cur_encoder_abs;   // ������һ�μƲ�ֵ��������̬�ʹ�̬�������ش����ã�
        cur_encoder_abs =  encoder.angle_abs; // ������һ�μƲ�ֵ
        
        //����ʵʱ���ʶ�ȡ���
        extern tGameInfo JudgeReceive;
        extern INA260 INA260_1;
        if(maxPower != JudgeReceive.MaxPower)  maxPower = JudgeReceive.MaxPower; 
        // �����ǰ�趨������ʲ��ǲ���ϵͳ��������ʣ��͸���һ��
        // Ŀ����Ϊ�˱���������ߵ���ʱ���ϳ�֮��ķ������ݲ�ͬ�Ĵ���
        PowerNow = INA260_1.Power / 1000.0f + 1.9;  //ʵ�����ϵͳ�����Ĺ���ֵ��INA260��Ҫ��Լ1.9W
        remainEnergy = JudgeReceive.remainEnergy;   //�������ϵͳ������ʣ������
             
        
        //λ���ںϸ��£��ù�翪�غ��г̿��صĴ����������������ļ���ֵ��
        if(LimitSw_State == Limit_Right_Active)  
            TIM_SetCounter(TIM3,22469),encoder.angle_abs = 22469;  //���Ҷ�
        if(LimitSw_State == Limit_Left_Active)
            TIM_SetCounter(TIM3,0),encoder.angle_abs = 0;  //�����
        if(Last_PhotoEle_State == PhotoEle_Both_IDLE 
        && Cur_PhotoEle_State == PhotoEle_Right_Active)  
            TIM_SetCounter(TIM3,21327),encoder.angle_abs = 21327;  //�Ҷ˹�翪�ش���
        if(Last_PhotoEle_State == PhotoEle_Both_IDLE 
        && Cur_PhotoEle_State == PhotoEle_Left_Active)
            TIM_SetCounter(TIM3,781),encoder.angle_abs = 781;   //��˹�翪�ش���
        
        //�ܻ�״̬����
        if(cur_remainHP<last_remainHP && JudgeReceive.hurt_type != 0x04) 
        {//��hunter_type���ж�������Ϊ���̳����ʶ���ɵĵ�Ѫ
            is_Under_Attack = 10000;  //�ܻ�״̬�ı����õ���̬�ݼ�����
            cur_escape_period = 0;    //���ܻ����ļ����õ�������
        }
        if(is_Under_Attack) is_Under_Attack--;  //���ȵݼ�
              

        //���ݵ���״ֵ̬ȷ�����ĸ�ִ�к���
        if (Sentry_State.Chassis_Mode == Chassis_Patrol) Chassis_Patrol_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_RC) Chassis_RC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_SLEEP) Chassis_SLEEP_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_DEBUG) Chassis_DEBUG_Act();

        vTaskDelay(1);
    }
}

/**
  * @brief  PCģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
uint8_t chassisFlag_Patrol_Init=0; //��ʼ���ı�־λ
static void Chassis_Patrol_Act(void)
{
    //����ֶα���
    static uint16_t  railThreshold_R;  
    static uint16_t  railThreshold_L;
    static uint8_t railSeg_Flag; //����״̬����־����L�Ρ�M�λ���R��

    //�м�α������
    static uint32_t mid_movingTick_RtoL; //�����жα���ļ�����
    static uint8_t mid_reverse_flag_RtoL;//��־���м���Ǳ���ǰ���Ǳ����
    static uint32_t mid_movingTick_LtoR; //�����жα���ļ�����
    static uint8_t mid_reverse_flag_LtoR;//��־���м���Ǳ���ǰ���Ǳ����


    if(chassisFlag_Patrol_Init == 0)
    {    
        railThreshold_L = 4000;
        railThreshold_R = 18469;
        srand(25);  //�����������ӳ�ʼ���͵��̵��˶������й�
        chassisFlag_Patrol_Init=1; 
    }
        

    
    //�����˶�״̬��������������
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
    {//���߶�δ����
//        if(motor_chassis.pid.SetPoint)
//        {//���������˶���waitingTick���㣬movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//������Ϊ��ͣ����ͬʱ����waitingTick��movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
    {//��ഥ������������J�����˶���ͣ��movingTick���㣬waitingTick++, �˶�����
        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
    }
    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
    {//�Ҳഥ������������J�����˶���ͣ��movingTick���㣬waitingTick++, �˶�����
        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
    }
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
    {//˫�ߴ���������Ϊ��Ϊ��ͣ���
        waitingTick = 1,movingTick = 0;//����������
    }
    

    //���µ����ڹ���ϵ���һ��
    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
    
    //���ݸ��ּ���������ֵ���ı��˶�״̬
    if(waitingTick>0 && waitingTick<100 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//��ͣ״̬0��
    else if(waitingTick>100 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);  //����������1��״̬ 
    else{//���ݼ��٣���ֹ����Ծ���ʹ����µ��̳����ʹҵ�
        if(waitingTick == 0 && movingTick > 60)
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬ 
        if(waitingTick == 0 && movingTick > 100)
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬
        if(waitingTick == 0 && movingTick > 200)
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);  //��2�ٿ�ʼ�ټ�һ���ٵ�3��״̬  
        if(waitingTick == 0 && movingTick > 300)
            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //��3�ٿ�ʼ�ټ�һ���ٵ�4��״̬ 
        if(waitingTick == 0 && movingTick > 350)
            motor_chassis.pid.SetPoint = (movingDir)?(-4300):(4300);    
        if(waitingTick == 0 && movingTick > 480 && movingTick < 810)  
            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);    
        //810ms���ٶν�����Ͱ�����ٶȵĿ���Ȩ������������
    }
    
    
    //�м��������� ///�ٺٺ�(��?����)
    if(!(rand() % 2))
    {
        if((last_encoder_abs < 14200)
          && (cur_encoder_abs >= 14200)
          && movingDir == MovingDir_R
          && !mid_reverse_flag_RtoL)//���������˶�
        {   
            mid_movingTick_RtoL=0;  //����м�μ�����
            motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800); //�ȼ�һ����        
            mid_reverse_flag_RtoL = 1; //�м�δ�����λ
        }
        if(mid_reverse_flag_RtoL)
        {//�м�η���󣬽��ݼ���
            mid_movingTick_RtoL++;
            
            if(mid_movingTick_RtoL >50 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_L; }//�ȱ����ټ���
            if(mid_movingTick_RtoL > 100)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            if(mid_movingTick_RtoL > 200)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_RtoL > 300)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_RtoL > 400)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_RtoL > 500)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_RtoL > 600)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_RtoL > 700)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);       
        }
        if(mid_movingTick_RtoL>1000) mid_reverse_flag_RtoL = 0,mid_movingTick_RtoL = 0;//�м�α����־�ĳ�ʱ��λ
    }
    if(!rand() % 2)
    {
        if((last_encoder_abs >= 9000)
          && (cur_encoder_abs < 9000)
          && movingDir == MovingDir_L
          && !mid_reverse_flag_LtoR)//���������˶�
        {   
            mid_movingTick_LtoR=0;  //����м�μ�����
            motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800); //�ȼ�һ����        
            mid_reverse_flag_LtoR = 1; //�м�δ�����λ
        }
        if(mid_reverse_flag_LtoR)
        {//�м�η����,���ݼ���
            mid_movingTick_LtoR++;
            if(mid_movingTick_LtoR >50 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_R; }//�ȱ����ټ���
            if(mid_movingTick_LtoR > 100)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            if(mid_movingTick_LtoR > 200)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_LtoR > 300)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_LtoR > 400)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_LtoR > 500)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_LtoR > 600)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_LtoR > 700)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);       
        }
        if(mid_movingTick_LtoR>1000) mid_reverse_flag_LtoR = 0,mid_movingTick_LtoR = 0;//�м�α����־�ĳ�ʱ��λ
    }   
    
    //�������˿��������εļ��٣����ñ�����ֵ���٣����ù�翪�صĴ���������
    if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    
    
//    Chassis_SLEEP_PID_Cal();
    if(is_Under_Attack || cur_escape_period < set_escape_period) Chassis_Patrol_PID_Cal();
    else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


/**
  * @brief  ң��ģʽ�µ��̵��˶�(�ĳɼ�¼ģʽ�ĵ����˶����˶����� ֻ���ǹ�翪�غ��г̿��ص� ��ȡ)
  * @param  None
  * @retval None
  */
static void Chassis_RC_Act(void)
{
//    static float kChassis = 4.0f;   //ҡ������ϵ��
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    //�����˶�״̬��������������
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
        waitingTick = 0,movingTick++;
    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
        waitingTick = 1, movingTick = 0;//����������


    //���ݼ���ֵ���ı��˶�״̬
    if(waitingTick>0 && waitingTick<100 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;
    else if(waitingTick>100 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);    

    if(movingDir == MovingDir_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    if(movingDir == MovingDir_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    
    /*if(is_Under_Attack || cur_escape_period < set_escape_period)*/ Chassis_RC_PID_Cal();
    //else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


inline static void Chassis_SLEEP_Act(void)
{
    Chassis_SLEEP_PID_Cal();
}

int16_t debug_SetPoint = 0;
static void Chassis_DEBUG_Act(void)
{   
    motor_chassis.pid.SetPoint = debug_SetPoint;
    Chassis_DEBUG_PID_Cal();
}

static void Chassis_RC_PID_Cal(void)
{
//    float Chassis_Iset;
//    
//    //����3508 �޷�+��ֵ
//    motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
//    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);

    Chassis_CAN1Send(0);
}

static void Chassis_Patrol_PID_Cal(void)
{
    //���ʿ������ ���㵱ǰ������ϵ��
    PowerLimit_k = LIMIT_MAX_MIN(maxPower / PowerNow, 2.7f, 0.0f);  //�õ�ǰ���ʺ�ʵ�ʹ��������һ������ϵ��
    if(remainEnergy <= 50) PowerLimit_k *=0.33f;  //���0.4�ö�����
    else if(remainEnergy >=150 && PowerLimit_k > 1.0f) PowerLimit_k *= PowerLimit_k;    //����ʣ��������������ϵ������һ������
    else if(remainEnergy <100 && remainEnergy >50 && PowerLimit_k >= 1.0f) PowerLimit_k = 1.0f; 
    
//    //������PID���
//    float Chassis_Iset;
//    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    Chassis_Iset = LIMIT_MAX_MIN(PowerLimit_k * Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(0);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN1Send(0);
}

int32_t debug_Chassis_Iset;
static void Chassis_DEBUG_PID_Cal(void)
{
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(0);
}

/**
  * @brief  ����PID��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
    motor_chassis.pid.P = 50.0f;//0.8f;
	motor_chassis.pid.I = 0;//0.6f;
	motor_chassis.pid.D = 0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
    motor_chassis.pid.OutMax = 5000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
}

void PID_Chassis_ClearError(void)
{  //����˵�ǿ��Բ�����������ģ���Ŀǰ����һ�������Է���һ
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
}


