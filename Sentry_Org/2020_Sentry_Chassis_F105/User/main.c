#include "main.h"

RCC_ClocksTypeDef rcc;
//uint8_t Mode;
Robot Sentry;
uint32_t syscnt;
Status_t Sentry_Status;
extern float Chassis_Iset;
//extern _2006_motor_t BodanMotor[2];
//extern int16_t Shoot_init_flag;

static void Hardware_Init(void);
static void System_Init(void);

//������
int main(void)
{	

	SysTick_Config(72000);
	Hardware_Init();
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ����Ӧ�ӿڵ�ʱ�ӣ��Լ�RCC_APB2Periph_AFIO
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     //����JTAG

	while(1){}
}

/**
 *  @brief  ��Ƭ���ϵ�ʱ���е�Ӳ����ʼ��
 *  @param  ��
 *  @retval ��
 */
static void Hardware_Init(void)
{
	RCC_GetClocksFreq(&rcc);//��ȡʱ���Ƿ�׼ȷ��F105Ϊ72M
//	USART1_Configuration();//Ԥ��������ϵͳ
    CAN1_Configuration();
    CAN2_Configuration();

	LED_Init();
	TIM3_Configuration_Encoder(); //AB���ȡ
	EXTI_Configuration_Encoder(); //Z�ഥ��
	TIM2_Configuration_Timer();
    //GPIO_Configuration_Limit_Switch();//�г̿�������
    GPIO_Configuration_Phtotelectric_Switch();//��翪������
	i2c_init();
//	delay_us(10000);
    
    System_Init();
}

/**
 *  @brief  Ӳ����ʼ��֮���״̬ϵͳ����
 *  @param  ��
 *  @retval ��
 */
static void System_Init(void)
{
    PID_Chassis_Init();
	Sentry.State = SENTRY_SCANNING;
	BodanMotor_Init();
	Sentry.Journey=START_POS;
    Sentry.MovingDir = FORWARD;
    remote_reset();
}


extern INA260 INA260_1;
extern float PowerNow;
/**
 *  @brief  ���ǰ̨�ж�
 *  @param  ��
 *  @retval ��
 */
extern int16_t EncoderA,EncoderB;
int8_t delayTick_bodan;
void SysTick_Handler(void)
{
   	syscnt++;
    
    EncoderA = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
    EncoderB = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
    
    
	//��״̬��
	Blink(1, 0);
	Blink(2, 1);
    Blink(3, 2);
	Blink(4, 3);
    
    //Encoder_detect();
	Read_Encoder();//��ȡ����������
    //limitSwitch_detect();//�г̿��ؼ��
	photoelectricSwitch_detect();//��翪�ؼ��
    
	//Read Power
    INA_READ_Vol();			
    INA_READ_Current();
	INA_READ_Power();//�����̹���
	PowerNow = INA260_1.Power/1000;
	///////////

	ChassisZeroCheck();
	Disconnect_Check();//���̵�����

    //action_update();
    Sentry_Status.ChassisMode = Chassis_SLEEP;
    Sentry_Status.ShootMode = Shoot_Up_PC|Shoot_Dn_SLEEP;
 
    //ChassisISend(0,0);
	if(syscnt%5 == 0)
	{
		if(Sentry_Status.ChassisMode == Chassis_SLEEP)
		{
			ChassisISend(0,0);
            remote_reset();
		}
		if(Sentry_Status.ChassisMode == Chassis_PC)		
		{
			Chassis_PC_Act();
		}
        if(Sentry_Status.ChassisMode == Chassis_RC)
        {
            Chassis_RC_Act();
        }
        if(Sentry_Status.ShootMode & Shoot_Up_PC || Sentry_Status.ShootMode & Shoot_Up_RC)
        {
            if(GetSysCnt()-delayTick_bodan>200) //��Ħ�����ȿ������ٿ��������
            Shoot_Fire_Cal();
        }
        else delayTick_bodan = GetSysCnt();
        
        if(Sentry_Status.ChassisMode != Chassis_SLEEP)
            Chassis_PID_Cal();
        
        ShootingHeat_CAN2Send();//����ǹ������
	}
	else if(syscnt%5 == 1)
    {
        //MasterSend();
        //ShootingHeat_CAN2Send();//����ǹ������
    }
}

/**
 *  @brief  ��ȡ��ǰʱ�����ֵ����ϵͳ�ϵ�֮����Ѿ����е�ʱ�䣩
 *  @param  ��
 *  @retval SysTick��������
 */
uint32_t GetSysCnt()
{
	return syscnt;
}

/**
  * @brief  ������ʱ(��)
  * @param  Ҫ��ʱ��ms��
  * @retval ��
  */
int32_t a=10300;
void idelay(uint32_t t)  
{
	int32_t i;
	for( i=0;i<t;i++)
	{
		a=1;
 		while(a!=0) a--;
	}
}
