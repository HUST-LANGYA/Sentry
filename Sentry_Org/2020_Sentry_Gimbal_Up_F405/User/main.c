#include "main.h"

uint32_t syscnt; //记录系统从上电之后的运行时长
extern Status_t Sentry_Status;
extern uint16_t PC_DownFlag;

RCC_ClocksTypeDef get_rcc_clock;

static void Hardware_Init(void);
static void System_Init(void);

//主函数
int main()
{
 	Hardware_Init(); //上电初始化

	SysTick_Config(168000);	//配置SysTick的时基，开启SysTick周期中断
//	delay_ms(10000);
//    RED_light_on();
//    BLUE_light_off();
	while(1)//裸机循环
	{
        RCC_GetClocksFreq(&get_rcc_clock); 
	}	
}

/**
 *  @brief  单片机上电时进行的硬件初始化
 *  @param  无
 *  @retval 无
 */
static void Hardware_Init(void)
{   
    TIM2_Configuration();  //延时TIM
    delay_ms(1000);  //等陀螺仪初始化完成
    LED_Configuration();//点上云台状态灯
    delay_ms(100);
    TIM4_Configuration();  //PWM输出给到上摩擦轮   
    delay_ms(100);
    FrictionWheel_Init();
	USART2_Configuration();  //和PC通信用
	delay_ms(100);
	USART3_Configuration();  //和遥控器通信用
	delay_ms(100);
	CAN_Configuration();	
	delay_ms(100);	
    
    delay_ms(600);
//	UART4_Configuration();
	System_Init();
}

/**
 *  @brief  硬件初始化之后的状态系统开启
 *  @param  无
 *  @retval 无
 */
static void System_Init(void)
{
	//BodanMoto_Init();  //上拨弹连在底盘上在
	Pid_Gimbal_Init();
	reset_remote_control_msg();
    
    //Sentry_Status.GimbalMode=Gimbal_Up_PC;
    //Sentry_Status.ShootMode=Shoot_Up_PC;
}

/**
 *  @brief  裸机前台中断
 *  @param  无
 *  @retval 无
 */
uint16_t testP=30000;
void SysTick_Handler(void)
{
	syscnt++;
	if(syscnt%1000 == 0) light_toggle();
	ZeroCheckHandle();
    Gyro_Can2Send(testP,testP);
	Gimbal_Limit();
	DisconnectDetect();  //掉线状态检测
    blink_state();       //根据掉线状态闪状态灯（这个函数还没有完成）
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
 *  @brief  获取当前时间计数值（即系统上电之后的已经运行的时间）
 *  @param  无
 *  @retval SysTick的周期数
 */
uint32_t GetSysCnt(void)
{
	return syscnt;
}

///**
//  * @brief  毫秒延时(软)
//  * @param  要延时的ms数
//  * @retval 无
//  */
//void delay_ms(uint32_t t)
//{
//	for(uint32_t i=0;i<t;i++)
//	{
//		int32_t a=10300;
// 		while(a--);
//	}
//}
