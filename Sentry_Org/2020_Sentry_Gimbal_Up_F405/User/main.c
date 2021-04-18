#include "main.h"

uint32_t syscnt; //��¼ϵͳ���ϵ�֮�������ʱ��
extern Status_t Sentry_Status;
extern uint16_t PC_DownFlag;

RCC_ClocksTypeDef get_rcc_clock;

static void Hardware_Init(void);
static void System_Init(void);

//������
int main()
{
 	Hardware_Init(); //�ϵ��ʼ��

	SysTick_Config(168000);	//����SysTick��ʱ��������SysTick�����ж�
//	delay_ms(10000);
//    RED_light_on();
//    BLUE_light_off();
	while(1)//���ѭ��
	{
        RCC_GetClocksFreq(&get_rcc_clock); 
	}	
}

/**
 *  @brief  ��Ƭ���ϵ�ʱ���е�Ӳ����ʼ��
 *  @param  ��
 *  @retval ��
 */
static void Hardware_Init(void)
{   
    TIM2_Configuration();  //��ʱTIM
    delay_ms(1000);  //�������ǳ�ʼ�����
    LED_Configuration();//������̨״̬��
    delay_ms(100);
    TIM4_Configuration();  //PWM���������Ħ����   
    delay_ms(100);
    FrictionWheel_Init();
	USART2_Configuration();  //��PCͨ����
	delay_ms(100);
	USART3_Configuration();  //��ң����ͨ����
	delay_ms(100);
	CAN_Configuration();	
	delay_ms(100);	
    
    delay_ms(600);
//	UART4_Configuration();
	System_Init();
}

/**
 *  @brief  Ӳ����ʼ��֮���״̬ϵͳ����
 *  @param  ��
 *  @retval ��
 */
static void System_Init(void)
{
	//BodanMoto_Init();  //�ϲ������ڵ�������
	Pid_Gimbal_Init();
	reset_remote_control_msg();
    
    //Sentry_Status.GimbalMode=Gimbal_Up_PC;
    //Sentry_Status.ShootMode=Shoot_Up_PC;
}

/**
 *  @brief  ���ǰ̨�ж�
 *  @param  ��
 *  @retval ��
 */
uint16_t testP=30000;
void SysTick_Handler(void)
{
	syscnt++;
	if(syscnt%1000 == 0) light_toggle();
	ZeroCheckHandle();
    Gyro_Can2Send(testP,testP);
	Gimbal_Limit();
	DisconnectDetect();  //����״̬���
    blink_state();       //���ݵ���״̬��״̬�ƣ����������û����ɣ�
	Status_Act();	
	
	if(Sentry_Status.GimbalMode != Gimbal_Up_SLEEP)
	{
		Gimbal_Pid_Cal();
		Shoot_Fire_Cal();
	}
	else
	{
		Gyro_Can2Send(0,0);
		//Bodan_Can1Send(0);
	}

	if(syscnt%5 == 0)
		PC_MsgSend(PC_SEND_OTHER); 
	else 
		PC_MsgSend(PC_SEND_GYRO);
	
//  SlaveMsgSend();
	
//	if(PC_DownFlag == 2)
//	{
//		PC_MsgSend(PC_SEND_POWER_DOWN);
//	}
}

/**
 *  @brief  ��ȡ��ǰʱ�����ֵ����ϵͳ�ϵ�֮����Ѿ����е�ʱ�䣩
 *  @param  ��
 *  @retval SysTick��������
 */
uint32_t GetSysCnt(void)
{
	return syscnt;
}

///**
//  * @brief  ������ʱ(��)
//  * @param  Ҫ��ʱ��ms��
//  * @retval ��
//  */
//void delay_ms(uint32_t t)
//{
//	for(uint32_t i=0;i<t;i++)
//	{
//		int32_t a=10300;
// 		while(a--);
//	}
//}
