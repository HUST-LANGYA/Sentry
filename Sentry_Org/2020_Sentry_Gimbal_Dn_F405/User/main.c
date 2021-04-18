#include "main.h"
uint32_t syscnt;
extern Status_t Sentry_Status;

int32_t FrictionSpeedSet[2]={850,850};
int16_t Test_friction_flag=0;

RCC_ClocksTypeDef get_rcc_clock;

static void Hardware_Init(void);
static void System_Init(void);

int main()
{
//	delay_ms(10000);

	Hardware_Init();

	SysTick_Config(168000);	
//	delay_ms(10000);
	while(1)
	{
        RCC_GetClocksFreq(&get_rcc_clock); 
	}	
}
void Hardware_Init(void)
{
	delay_ms(100);
	USART2_Configuration();  //和PC通信
	delay_ms(100);
    LED_Configuration();//点亮状态灯
    delay_ms(100);
    FrictionWheel_Set(0);
    TIM8_Configuration();    //摩擦轮的TIM
    delay_ms(60000);
    FrictionWheel_Set(400);
//	USART3_Configuration();
	delay_ms(100);
	CAN1_Configration();
	delay_ms(100);
	CAN2_Configration();	
	delay_ms(100);

//    delay_ms(40000);

	System_Init();
}
void System_Init(void)
{
	BodanMoto_Init();
	Pid_Gimbal_Init();
    remote_reset();
}
void SysTick_Handler(void)
{
	syscnt++;
	
    ZeroCheckHandle();
	Gimbal_Limit();
	DisconnectDetect();
    Status_Act();	
	
	if(Sentry_Status.GimbalMode != Gimbal_Dn_SLEEP)
	{
        Gimbal_PID_Cal();
//      FrictionWheel_Set(300);
    }
    else 
//	else
//	Gyro_Can2Send(0,0);
		
	Shoot_Fire_Cal();
	SlaveMsgSend();
}

uint32_t GetSysCnt(void)
{
	return syscnt;
}
/**
  * @brief  毫秒延时
  * @param  None
  * @retval None
  */
void delay_ms(uint32_t t)
{
	int32_t i;
	for( i=0;i<t;i++)
	{
		int32_t a=10300;
 		while(a--);
	}
}
