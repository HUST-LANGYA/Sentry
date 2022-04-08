#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
_2006_motor_t BodanMotor;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;
extern tGameInfo JudgeReceive;
//��ʼ����صı�־λ
uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

//��翪�ش������־
uint16_t Photo_Error_Flag=0;

//�ܻ��������
extern uint16_t last_remainHP,cur_remainHP;
extern uint16_t is_Under_Attack; //����ܻ����򽫸ñ�������ĳ������̬��ֵ
extern uint8_t armor_state;

//���ʲ���
float maxPower = 30.0f;  //�ȸĳ�20W
float PowerNow = 0.0f;
float PowerLimit_k=0.8f; //��������ϵ��
float PowerBack;
float Chassis_Iset_test=0;
uint16_t remainEnergy;
float Balance_Power;
PID_Typedef PowerLimit;
PID_Typedef PowerLimit_1;

//�˶�״̬����
MovingDir_t movingDir;//��ǰ�н��ķ���  1Ϊ���� 0Ϊ����  
uint16_t waitingTick;//�ڶ�ͷ��ͣ��ʱ��
uint32_t movingTick;//�˶��м�����
int16_t last_encoder_abs,cur_encoder_abs;

//λ���ں���ر���
extern LimitSWState_t LimitSw_State;//�г̿���״̬
extern FtirState_t Ftir_State;  //��翪��״̬
extern FtirState_t Last_PhotoEle_State,Cur_PhotoEle_State;//��������������ĸ���
extern _encoder_t encoder;
int32_t encoder_distance = 0;//�ñ�������¼�����ٶ�֮ǰ��������
int32_t encoder_round = 0; //��¼������ת����Ȧ��
    
//�ܻ���������� ���ⲿ���ӣ�
uint8_t set_escape_period = 4;  //�ܻ����Ѳ��������
uint8_t cur_escape_period = 0; //��ǰѲ�ߵ������� ��һ���ܻ���������������㣬Ȼ�����cur��set�в��һֱѲ��
//���Ҳ��翪�ش���������

//ģ������ʱֹͣ���
uint8_t aim_flag=0;

//�⼸��������ÿ��shoot_Act���õģ��Ժ�Ҳ����Ըĳ�һ���ṹ�壬Ȼ����Ե�Act�ø��Եĳ�Աֵ
uint32_t shootDelayTick;         //��¼������ģʽ�£�ÿ�����ӵ�֮���ʵʱʱ����
float testInc = 29510.0f;        //26910.0f;//��һ�ŵ��������pos������ֵ
uint32_t delayTick_oneShot = 50; //�ߵ�һ�ŵ�������ʱ��(ms)
float bodanLastPos;              //����ϴε�������ʱ�Ĳ������λ��ֵ
float debug_setpoint = 0;
//��ת������
uint8_t block_bullet_flag;     //������־ ����λʱ��ʾ�����˿�������Ҫ�����̷�ת����
int16_t block_bullet_tick = 0; //���������������������������һ��������������Ϊ��������λ������־��ִ�з�ת
uint16_t Shoot_Enable_cnt;
uint8_t Shoot_Enable_flag;

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
  	  encoder.zerocheck.CountCycle = 65535; //��Ȼ�����ʼ���˱�����������Ľṹ�壬����ʵ�⣬2021�����Ĺ���������int16_t���ȣ����Թ�������û�������õģ�ע�⵽������65535
  	  encoder.zerocheck.LastValue = encoder.angle_abs;
   	  encoder.zerocheck.Circle = 0;
//    
    //�˶����Գ�ʼ������
    cur_escape_period = set_escape_period; //�趨��ʼʱ��������ܣ�����ֹ����
    
    while (1)
    {
        //ǹ���������ƵĲ���
        //TODO! ���԰������������Ĳ��ֵַ�һ��������������
        HeatUpdate_0();   //����ǹ������ֵ��������������ֵ����ͨ��CAN���͵�������̨����ȥ
        HeatUpdate_2();   //17mmǹ�ܵ�ID�Ű�ż��������
        ShootingHeat_CAN2Send();    //���͸�������̨���������� //�����Ǹ��߰�������(9��11��)
        
        //��������λ���ں����
        encoder.angle_abs = Read_Encoder();   // ��TIM��CNT�Ĵ������³��µļƲ�ֵ
        last_encoder_abs = cur_encoder_abs;   // ������һ�μƲ�ֵ��������̬�ʹ�̬�������ش����ã�
        cur_encoder_abs =  encoder.angle_abs; // ������һ�μƲ�ֵ
        
        //����ʵʱ���ʶ�ȡ���
        extern tGameInfo JudgeReceive;
        extern INA260 INA260_1;
			  INA_READ_Power();
        //if(maxPower != JudgeReceive.MaxPower)  maxPower = JudgeReceive.MaxPower; 
        // �����ǰ�趨������ʲ��ǲ���ϵͳ��������ʣ��͸���һ��
        // Ŀ����Ϊ�˱���������ߵ���ʱ���ϳ�֮��ķ������ݲ�ͬ�Ĵ���
        PowerNow = INA260_1.Power / 1000.0f + 1.3;  //ʵ�����ϵͳ�����Ĺ���ֵ��INA260��Ҫ��Լ1.9W
        remainEnergy = JudgeReceive.remainEnergy;   //�������ϵͳ������ʣ������
         
        
        //λ���ںϸ��£��ù�翪�غ��г̿��صĴ����������������ļ���ֵ��
//        if(LimitSw_State == Limit_Right_Active)  
//            TIM_SetCounter(TIM3,22469),encoder.angle_abs = 22469;  //���Ҷ�
//        if(LimitSw_State == Limit_Left_Active)
//            TIM_SetCounter(TIM3,0),encoder.angle_abs = 0;  //�����
//        if(Last_PhotoEle_State == Ftir_Both_IDLE 
//        && Cur_PhotoEle_State == Ftir_Right_Active)  
//            TIM_SetCounter(TIM3,21327),encoder.angle_abs = 21327;  //�Ҷ˹�翪�ش���
//        if(Last_PhotoEle_State == Ftir_Both_IDLE 
//        && Cur_PhotoEle_State == Ftir_Left_Active)
//            TIM_SetCounter(TIM3,781),encoder.angle_abs = 781;   //��˹�翪�ش���
        
				if(armor_state == 0x30 && motor_chassis.pid.SetPoint == 0)
				{
				   aim_flag = 1;
				}
        //�ܻ�״̬����
//        if(cur_remainHP<last_remainHP && JudgeReceive.hurt_type != 0x04) 
//        {//��hunter_type���ж�������Ϊ���̳����ʶ���ɵĵ�Ѫ
//            is_Under_Attack = 10000;  //�ܻ�״̬�ı����õ���̬�ݼ�����
//            cur_escape_period = 0;    //���ܻ����ļ����õ�������
//        }
//        if(is_Under_Attack) is_Under_Attack--;  //���ȵݼ�
//              

				//������ת���
        if (ABS(BodanMotor.RealSpeed) < 5 && ABS(BodanMotor.I_Set) > 1000)
            block_bullet_tick++; //��ת����
        else
            block_bullet_tick = (block_bullet_tick > 0) ? (block_bullet_tick - 1) : 0; //��ת�����ĵ�������
        if (block_bullet_tick >= 3000)
            block_bullet_flag = 1; //��ת3s,����Ϊ��ת����־��λ
        if (block_bullet_tick <= 0)
            block_bullet_flag = 0; //ֻ�м�������֮�󣬲���Ϊ���ٶ�ת
				
				Shoot_Enable_cnt = (Shoot_Enable_cnt>1000)? (Shoot_Enable_cnt) : (Shoot_Enable_cnt+1);
				
        //Bodan_CAN1Send(0);
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
//����ֶα���
static uint16_t  railThreshold_R;  
static uint16_t  railThreshold_L;
static uint8_t railSeg_Flag; //����״̬����־����L�Ρ�M�λ���R��

//�м�α������
uint32_t mid_movingTick_RtoL; //�����жα���ļ�����
uint8_t mid_reverse_flag_RtoL;//��־���м���Ǳ���ǰ���Ǳ����
uint32_t mid_movingTick_LtoR; //�����жα���ļ�����
uint8_t mid_reverse_flag_LtoR;//��־���м���Ǳ���ǰ���Ǳ����

static void Chassis_Patrol_Act(void)
{
//    //����ֶα���
//    static uint16_t  railThreshold_R;  
//    static uint16_t  railThreshold_L;
//    static uint8_t railSeg_Flag; //����״̬����־����L�Ρ�M�λ���R��

//    //�м�α������
//    static uint32_t mid_movingTick_RtoL; //�����жα���ļ�����
//    static uint8_t mid_reverse_flag_RtoL;//��־���м���Ǳ���ǰ���Ǳ����
//    static uint32_t mid_movingTick_LtoR; //�����жα���ļ�����
//    static uint8_t mid_reverse_flag_LtoR;//��־���м���Ǳ���ǰ���Ǳ����


    if(chassisFlag_Patrol_Init == 0)
    {    
        railThreshold_L = 4000;
        railThreshold_R = 18469;
        srand(25);  //�����������ӳ�ʼ���͵��̵��˶������й�
        chassisFlag_Patrol_Init=1; 
    }
        
		 

    
    //�����˶�״̬��������������
    if(Ftir_State == Ftir_Both_IDLE || LimitSw_State == Limit_Both_IDLE) 
    {//���߶�δ����
//        if(motor_chassis.pid.SetPoint)
//        {//���������˶���waitingTick���㣬movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//������Ϊ��ͣ����ͬʱ����waitingTick��movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
    else if(LimitSw_State == Limit_Left_Active || LimitSw_State == Limit_Right_Active )  //�г̿��ش���
    {
			  movingDir = (LimitSw_State == Limit_Left_Active)? (MovingDir_R) : (MovingDir_L);
        waitingTick=0,movingTick = 0;
    }
    else if(Ftir_State == Ftir_Right_Active || Ftir_State == Ftir_Left_Active ) //��翪�ش���,����ʼ���ٵĹ���
    {
			 
        waitingTick++,movingTick = 0;
    }
    else if(Ftir_State == Ftir_Both_Active ) 
    {//˫�ߴ���������Ϊ��Ϊ��ͣ���
        waitingTick = 1,movingTick = 0;//����������
    }
    
		

    //���µ����ڹ���ϵ���һ��
    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
		
		//����״̬�ж�
		 Photo_Error_Flag=((Ftir_State==Ftir_Right_Active)&&(railSeg_Flag==RailSeg_M));
    
    //���ݸ��ּ���������ֵ���ı��˶�״̬
    if(waitingTick>=0 && waitingTick<50 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//��ͣ״̬0��
    else if(waitingTick>50 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);  //����������1��״̬ 
    else{//���ݼ��٣���ֹ����Ծ���ʹ����µ��̳����ʹҵ�
        if(waitingTick == 0 && movingTick > 10)
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬ 
        if(waitingTick == 0 && movingTick > 20)
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬
        if(waitingTick == 0 && movingTick > 30)
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);  //��2�ٿ�ʼ�ټ�һ���ٵ�3��״̬  
        if(waitingTick == 0 && movingTick > 50)
            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //��3�ٿ�ʼ�ټ�һ���ٵ�4��״̬ 
        if(waitingTick == 0 && movingTick > 60)
            motor_chassis.pid.SetPoint = (movingDir)?(-4300):(4300);    
        if(waitingTick == 0 && movingTick > 70 /*&& movingTick < 100*/)  
            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);  
        if(waitingTick == 0&& movingTick>100 && movingTick<150)
					motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);					
        //810ms���ٶν�����Ͱ�����ٶȵĿ���Ȩ������������
    }
    
    
    //�м��������� ///�ٺٺ�(��?����)
				
		
    if(!(rand() % 2))
    {
        if((last_encoder_abs < 14200)
          && (cur_encoder_abs >= 14200)
          && movingDir == MovingDir_R
          && !mid_reverse_flag_RtoL)//���������˶���û�о����ұߵı����
        {   
            mid_movingTick_RtoL=0;  //����м�μ�����
            motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); //�ȼ�һ����        
            mid_reverse_flag_RtoL = 1; //�м�δ�����λ
        }
        if(mid_reverse_flag_RtoL)
        {//�м�η���󣬽��ݼ���
            mid_movingTick_RtoL++;
            
            if(mid_movingTick_RtoL >10 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_L; }//�ȱ����ټ���
            //if(mid_movingTick_RtoL > 20)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            //if(mid_movingTick_RtoL > 30)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_RtoL > 40)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_RtoL > 50)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_RtoL > 60)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_RtoL > 70)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_RtoL > 80)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);       
						if(mid_movingTick_RtoL > 90)  motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);
        }
        if(mid_movingTick_RtoL>600) mid_reverse_flag_RtoL = 0,mid_movingTick_RtoL = 0;//�м�α����־�ĳ�ʱ��λ
    }
		
		 // �����ʵ��
			 if(rand() % 2)
    {
        if((last_encoder_abs >= 12000) //�������óɺ�14200�����Ҫ��Ϊ���ܹ��ڶ�ܴ����ʱ��̾�����ʵ�ֶ������
          && (cur_encoder_abs < 12000)
          && movingDir == MovingDir_L
          && !mid_reverse_flag_LtoR)//���������˶�
        {   
            mid_movingTick_LtoR=0;  //����м�μ�����
            motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); //�ȼ�һ����        
            mid_reverse_flag_LtoR = 1; //�м�δ�����λ
        }
        if(mid_reverse_flag_LtoR)
        {//�м�η����,���ݼ���
            mid_movingTick_LtoR++;
            if(mid_movingTick_LtoR >10 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_R; }//�ȱ����ټ���
            //if(mid_movingTick_LtoR > 20)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            //if(mid_movingTick_LtoR > 30)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_LtoR > 40)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_LtoR > 50)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_LtoR > 60)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_LtoR > 70)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_LtoR > 80)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);     
            if(mid_movingTick_LtoR > 90)  motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800)						;
        }
        if(mid_movingTick_LtoR>600) mid_reverse_flag_LtoR = 0,mid_movingTick_LtoR = 0;//�м�α����־�ĳ�ʱ��λ
    }   
		
		
		
		
		
    //�������˿��������εļ��٣����ñ�����ֵ���٣����ù�翪�صĴ���������
    //if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); 
    //if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); 
		
    //��׼��ʱ��ͣ����
    //if(armor_state == 0x30)   motor_chassis.pid.SetPoint = 0;
//    Chassis_SLEEP_PID_Cal();
    if(is_Under_Attack /*|| cur_escape_period < set_escape_period*/) Chassis_Patrol_PID_Cal(); // is_Under_Attack��������Ϊ������
    else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


/**
  * @brief  ң��ģʽ�µ��̵��˶�(�ĳɼ�¼ģʽ�ĵ����˶����˶����� ֻ���ǹ�翪�غ��г̿��ص� ��ȡ)
  * @param  None
  * @retval None
  */
static void Chassis_RC_Act(void)
{
    static float kChassis = 10.0f;   //ҡ������ϵ��
	  
	  RC_Ctl_t RC_Ctl = getRCData();
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
			  railThreshold_L = 4000;
        railThreshold_R = 18469;
    }
    
		 //�����˶�״̬��������������
   if(Ftir_State == Ftir_Both_IDLE || LimitSw_State == Limit_Both_IDLE) 
    {//���߶�δ����
//        if(motor_chassis.pid.SetPoint)
//        {//���������˶���waitingTick���㣬movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//������Ϊ��ͣ����ͬʱ����waitingTick��movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
    else if(LimitSw_State == Limit_Left_Active || LimitSw_State == Limit_Right_Active )  //�г̿��ش���
    {
			  movingDir = (LimitSw_State == Limit_Left_Active)? (MovingDir_R) : (MovingDir_L);
        waitingTick=0,movingTick = 0;
    }
    else if(Ftir_State == Ftir_Right_Active || Ftir_State == Ftir_Left_Active ) //��翪�ش���,����ʼ���ٵĹ���
    {
			 
        waitingTick++,movingTick = 0;
    }
    else if(Ftir_State == Ftir_Both_Active ) 
    {//˫�ߴ���������Ϊ��Ϊ��ͣ���
        waitingTick = 1,movingTick = 0;//����������
    }
    
    
		

    //���µ����ڹ���ϵ���һ��
    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
		
		//����״̬�ж�,������Ϊ�ڳ�ʱ����˶��л������ʱ�����ڵ����
		 Photo_Error_Flag=((Ftir_State==Ftir_Right_Active)&&(railSeg_Flag==RailSeg_M));
		
    //�м�ش�ֱ����ң�������趨�ٶȵ�Ŀ��ֵ
		//motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch0-1024);//ch0�����ٶ�ֵ 
//		 motor_chassis.pid.SetPoint =  (movingDir)?(-1500):(1500);
//		 if(Ftir_State == Ftir_Both_Active ) 
//		 {
//		    motor_chassis.pid.SetPoint =  0;
//		 }
		 
		 //���ݸ��ּ���������ֵ���ı��˶�״̬
    if(waitingTick>=0 && waitingTick<50 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//��ͣ״̬0��
    else if(waitingTick>50 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);  //����������1��״̬ 
    else{//���ݼ��٣���ֹ����Ծ���ʹ����µ��̳����ʹҵ�
        if(waitingTick == 0 && movingTick > 10)
            motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬ 
//        if(waitingTick == 0 && movingTick > 20)
//            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);  //��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬
//        if(waitingTick == 0 && movingTick > 30)
//            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);  //��2�ٿ�ʼ�ټ�һ���ٵ�3��״̬  
//        if(waitingTick == 0 && movingTick > 50)
//            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //��3�ٿ�ʼ�ټ�һ���ٵ�4��״̬ 
//        if(waitingTick == 0 && movingTick > 60)
//            motor_chassis.pid.SetPoint = (movingDir)?(-4300):(4300);    
//        if(waitingTick == 0 && movingTick > 70 /*&& movingTick < 100*/)  
//            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);  
//        if(waitingTick == 0&& movingTick>100 && movingTick<150)
//					motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);					
//        //810ms���ٶν�����Ͱ�����ٶȵĿ���Ȩ������������
    }
    
		 
		//�������ߵ������ٶ��趨ֱֵ�����ó�3000
		//if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    //if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
		
    //�����˶�״̬��������������
//    if(PhotoEle_State == PhotoEle_Both_IDLE) 
//        waitingTick = 0,movingTick++;
//    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
//        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
//    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
//        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
//    else if(PhotoEle_State == PhotoEle_Both_Active ) 
//        waitingTick = 1, movingTick = 0;//����������


//    //���ݼ���ֵ���ı��˶�״̬
//    if(waitingTick>0 && waitingTick<100 && movingTick == 0) 
//        motor_chassis.pid.SetPoint = 0;
//    else if(waitingTick>100 && movingTick == 0)
//        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);    
//     
//    if(movingDir == MovingDir_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
//    if(movingDir == MovingDir_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    
    /*if(is_Under_Attack || cur_escape_period < set_escape_period)*/ Chassis_RC_PID_Cal();
    //else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


inline static void Chassis_SLEEP_Act(void)
{
	  
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
	
		//	}
				
	
	
	
    Chassis_SLEEP_PID_Cal();
}

int16_t debug_SetPoint = 0;
static void Chassis_DEBUG_Act(void)
{     
	  float Debug_Chassis_k=5.0f;
	  RC_Ctl_t RC_Ctl = getRCData();
	  debug_SetPoint=Debug_Chassis_k*(RC_Ctl.rc.ch0-1024);
    motor_chassis.pid.SetPoint = debug_SetPoint;
    Chassis_DEBUG_PID_Cal();
}

static float EnergyDanger=20;   //��������Σ��ֵ
static float Excess_Power=80;  //���õĳ��޹���//��С����Ҫ�����޲�ס����Ϊ������ʱ��̫����
//float Chassis_Iset;
float Chassis_V_Rc_Cal=0.00168f;
float Power_Rc_Cal=0.0f;
static void Chassis_RC_PID_Cal(void)
{
    float Chassis_Iset;
	  extern INA260 INA260_1;
	   
	  motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);
			
	   //Power_Rc_Cal=Chassis_Iset*Chassis_V_Rc_Cal;
//	  if(JudgeReceive.remainEnergy<20)
//		PowerLimit.SetPoint=JudgeReceive.MaxPower;
//    else
//    PowerLimit.SetPoint=Excess_Power*(JudgeReceive.remainEnergy-EnergyDanger)/(200-EnergyDanger);			
//		
		 
//		while(PowerNow>PowerLimit.SetPoint)  //���ù��ʻ�ʵ�ֹ���˥��
//		{
//		     PowerLimit_k+= PID_Calc(&PowerLimit,PowerNow) / PowerLimit.SetPoint;
//			   PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k,1,0);
//			   Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
//			   Chassis_Iset=PowerLimit_k*Chassis_Iset;		
//		}
	 
    //����3508 �޷�+��ֵ
    //motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    //Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);

    Chassis_CAN1Send(Chassis_Iset);
}
//uint16_t Power_De_Time=0;
float Chassis_V=0.024f;
float Chassis_V_Cal=0.0168f;
uint16_t Deline_Flag=0;
short Power_Time=0;
float Power_Cal=0.0f;
float Debug_Iset=0.0f;
static void Chassis_Patrol_PID_Cal(void)
{ 
	       float Chassis_Iset;
	       //short Power_De_Time=0;  //�ڲ�����
	  extern INA260 INA260_1;
	  //motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);
	
	  //if(JudgeReceive.remainEnergy<20)
	  if(remainEnergy<20) //��������ֵѡ��
		{
		PowerLimit.SetPoint=18.0f;
		}
		
	  //else if(movingTick<=100||mid_movingTick_RtoL<=150||mid_movingTick_LtoR<=150)  //��Щ��Ҫ���ݻ�����������һ��ȷ�������ٶ�ȷ��Ϊ���ٺ���
		else if(motor_chassis.real_speed<6000&&motor_chassis.real_speed>-6000)
		{
		PowerLimit.SetPoint=Excess_Power*(remainEnergy-EnergyDanger)/(200-EnergyDanger);
		}
		else if(motor_chassis.real_speed>6000||motor_chassis.real_speed<-6000)
    {			
		PowerLimit.SetPoint=(maxPower*0.9f); //���ٶεĹ���Ҫ��
		}
	  Chassis_Iset_test = PowerLimit.SetPoint/(Chassis_V); //�и��������ʵ�ʵĵ������������ٶȻ������ֵ
    //else
    //PowerLimit.SetPoint=Excess_Power*(JudgeReceive.remainEnergy-EnergyDanger)/(200-EnergyDanger);			
	   //if(movingTick>100||mid_movingTick_RtoL>150||mid_movingTick_LtoR>150)  //����һ�����ٶεĹ������
//		if(motor_chassis.real_speed>5200||motor_chassis.real_speed<-5200) //�����Ҫ��Ϊ�˹۲����ٶεĹ���ʹ�����
//			 Balance_Power=PowerNow;                                             
//		 else
//			 Balance_Power=30.0f;   //0.9*30.0
		 
		  //Power_Cal=Chassis_Iset*Chassis_V_Cal;   //���м��㹦��ֵ
		//while(PowerNow>PowerLimit_1.SetPoint&&Power_De_Time<8)  //���ù��ʻ�ʵ�ֹ���˥��,֮ǰ��while
	  if(PowerNow>PowerLimit.SetPoint&&Power_Time<8)
	 {       
		     Chassis_Iset=PowerLimit_k*Chassis_Iset;
			   Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
		     INA_READ_Power();
		     PowerNow=INA260_1.Power; //��ȡʵʱ����ֵ
		     //Power_Cal=Chassis_Iset*Chassis_V_Cal;
			   PowerBack=PID_Calc(&PowerLimit,PowerNow); 
		     PowerLimit_k+=(PowerBack/PowerLimit.SetPoint);
			   PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k,1,0); // ֮ǰû���޷�,��Ҫ�Ǹо������ͣ�����������
		     //Power_Cal=Chassis_Iset*Chassis_V;
			   Power_Time++;	
         Deline_Flag = !Deline_Flag;	 //ȡ����˥���ı�־		
		}
//		 PowerLimit_k=0.8f; //˥��ϵ����ԭ
//		 Power_Time=0;  //˥������
		 
		 else if(PowerNow>PowerLimit.SetPoint && Power_Time == 8) //��˥���������ǻ���û�н��͵�����������
		{
		   Chassis_Iset=Chassis_Iset*(PowerLimit.SetPoint/PowerNow);
			 Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
		}
		 else if(PowerNow < PowerLimit.SetPoint)
		{
		   Power_Time = 0;  
		   PowerLimit_k = 0.8f;
		}
		
		  //Chassis_V=PowerNow/Chassis_Iset; //ʵ�������ֵ�����ǳ���
//    //���ʿ������ ���㵱ǰ������ϵ��
//    PowerLimit_k = LIMIT_MAX_MIN(maxPower / PowerNow, 2.7f, 0.0f);  //�õ�ǰ���ʺ�ʵ�ʹ��������һ������ϵ��
//    if(remainEnergy <= 50) PowerLimit_k *=0.33f;  //���0.4�ö�����
//    else if(remainEnergy >=150 && PowerLimit_k > 1.0f) PowerLimit_k *= PowerLimit_k;    //����ʣ��������������ϵ������һ������
//    else if(remainEnergy <100 && remainEnergy >50 && PowerLimit_k >= 1.0f) PowerLimit_k = 1.0f; 
//    
//    //������PID���
//    float Chassis_Iset;
//    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    Chassis_Iset = LIMIT_MAX_MIN(PowerLimit_k * Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    Debug_Iset=Chassis_Iset;
    Chassis_CAN1Send(Chassis_Iset);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
	 Shoot_Enable_flag = (Shoot_Enable_cnt>=1000);   
	 
	   float fsend;
    BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos, BodanMotor.Angle_Inc); //��λ�û����ٶȻ��趨ֵ
    fsend = PID_Calc(&BodanMotor.pid_speed, BodanMotor.RealSpeed);                       //���ٶȻ������ֵ���
	  BodanMotor.I_Set = (Shoot_Enable_flag)?((int16_t)LIMIT_MAX_MIN(fsend, 10000, -10000)) : (0);
    //Bodan_Can2Send(BodanDelay_OVER ? BodanMotor.I_Set : 0); //����������
	  Bodan_CAN2Send(BodanMotor.I_Set);
	 // Bodan_CAN2Send(0);
	
	
    Chassis_CAN1Send(0);
	  //Bodan_CAN1Send(0);
}

int32_t debug_Chassis_Iset;
static void Chassis_DEBUG_PID_Cal(void)
{
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(debug_Chassis_Iset);
}

/**
  * @brief  ����PID��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
  motor_chassis.pid.P = 10.0f;//52.0f;//50.0f;//0.8f;
	motor_chassis.pid.I = 0;//0.6f;//0;//0.6f;
	motor_chassis.pid.D = 0;//0.2f;//0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
    motor_chassis.pid.OutMax = 10000.0f;//5000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
	//���ʻ�����
	PowerLimit.SetPoint=30.0f;  //������30W��ʱ������ϲ��õ���������
	PowerLimit.P= 0.001f;//0.001f;
	PowerLimit.I=0.01f; //0.0005f;
	PowerLimit.D= 0.001f;
	PowerLimit.IMax=0.5f;
	PowerLimit.OutMax=15.0f;  //�������
	
	PowerLimit_1.SetPoint=20.0f;
	PowerLimit_1.P=0.3f; //0.004f;
	PowerLimit_1.I=0.02f;
	PowerLimit_1.D=0;//0.009f;
	PowerLimit_1.IMax=0.5f;
	PowerLimit_1.OutMax=10.0f;
	
	//�ϲ���
	  BodanMotor.pid_speed.P = 1.5f;
    BodanMotor.pid_speed.I = 0.0f;
    BodanMotor.pid_speed.D = 0.5f;
    BodanMotor.pid_speed.IMax = 0.0f;
    BodanMotor.pid_speed.OutMax = 100000.0f;
 
    BodanMotor.pid_pos.P = 1.0f;//0.2f;
    BodanMotor.pid_pos.I = 0.0f;
    BodanMotor.pid_pos.D = 0.0f;
    BodanMotor.pid_pos.IMax = 0.0f;
    BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;
    BodanMotor.pid_pos.OutMax = 1000000.0f;
		
    BodanMotor.zerocheck.Circle = 0;
    BodanMotor.zerocheck.CountCycle = 8191;
    BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}

void PID_Chassis_ClearError(void)
{  //����˵�ǿ��Բ�����������ģ���Ŀǰ����һ�������Է���һ
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
}

//void ReadINA(void)
//{
//     INA_READ_Current();
//     INA_READ_Vol();
//     INA_READ_Power();

//}

