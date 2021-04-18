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

//主函数
int main(void)
{	

	SysTick_Config(72000);
	Hardware_Init();
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //使能相应接口的时钟，以及RCC_APB2Periph_AFIO
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     //禁用JTAG

	while(1){}
}

/**
 *  @brief  单片机上电时进行的硬件初始化
 *  @param  无
 *  @retval 无
 */
static void Hardware_Init(void)
{
	RCC_GetClocksFreq(&rcc);//读取时钟是否准确，F105为72M
//	USART1_Configuration();//预留给裁判系统
    CAN1_Configuration();
    CAN2_Configuration();

	LED_Init();
	TIM3_Configuration_Encoder(); //AB相读取
	EXTI_Configuration_Encoder(); //Z相触发
	TIM2_Configuration_Timer();
    //GPIO_Configuration_Limit_Switch();//行程开关配置
    GPIO_Configuration_Phtotelectric_Switch();//光电开关配置
	i2c_init();
//	delay_us(10000);
    
    System_Init();
}

/**
 *  @brief  硬件初始化之后的状态系统开启
 *  @param  无
 *  @retval 无
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
 *  @brief  裸机前台中断
 *  @param  无
 *  @retval 无
 */
extern int16_t EncoderA,EncoderB;
int8_t delayTick_bodan;
void SysTick_Handler(void)
{
   	syscnt++;
    
    EncoderA = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
    EncoderB = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
    
    
	//闪状态灯
	Blink(1, 0);
	Blink(2, 1);
    Blink(3, 2);
	Blink(4, 3);
    
    //Encoder_detect();
	Read_Encoder();//读取编码器数据
    //limitSwitch_detect();//行程开关检测
	photoelectricSwitch_detect();//光电开关检测
    
	//Read Power
    INA_READ_Vol();			
    INA_READ_Current();
	INA_READ_Power();//读底盘功率
	PowerNow = INA260_1.Power/1000;
	///////////

	ChassisZeroCheck();
	Disconnect_Check();//底盘掉电检测

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
            if(GetSysCnt()-delayTick_bodan>200) //等摩擦轮先开起来再开拨弹电机
            Shoot_Fire_Cal();
        }
        else delayTick_bodan = GetSysCnt();
        
        if(Sentry_Status.ChassisMode != Chassis_SLEEP)
            Chassis_PID_Cal();
        
        ShootingHeat_CAN2Send();//发送枪口热量
	}
	else if(syscnt%5 == 1)
    {
        //MasterSend();
        //ShootingHeat_CAN2Send();//发送枪口热量
    }
}

/**
 *  @brief  获取当前时间计数值（即系统上电之后的已经运行的时间）
 *  @param  无
 *  @retval SysTick的周期数
 */
uint32_t GetSysCnt()
{
	return syscnt;
}

/**
  * @brief  毫秒延时(软)
  * @param  要延时的ms数
  * @retval 无
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
