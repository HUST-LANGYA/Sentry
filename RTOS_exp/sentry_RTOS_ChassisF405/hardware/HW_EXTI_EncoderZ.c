#include "main.h"
#include "HW_EXTI_EncoderZ.h"

//尚未测试该底层是否时正确的

extern block_disconnect_t block_disconnect;

/**
 * @brief  外部中断配置函数(编码器过零触发)
 * @param  None
 * @retval None
 */
void EXTI_Config_Encoder(void)
{
    /*
    可能用到的外部中断有：
        编码器的机械零位相输入
        两侧行程开关的触发
    */
	GPIO_InitTypeDef gpioInit; 
	EXTI_InitTypeDef extiInit;
	NVIC_InitTypeDef nvicInit;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	gpioInit.GPIO_Pin = GPIO_Pin_10; 
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpioInit); 
  
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);  
    extiInit.EXTI_Line = EXTI_Line10; 
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt; 
    extiInit.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发（当转过机械零位时，该线上会输入一个高电平进来） 
    extiInit.EXTI_LineCmd = ENABLE;
    EXTI_Init(&extiInit); 
    EXTI_GenerateSWInterrupt(EXTI_Line10); 
	
	nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&nvicInit);
}


int32_t last_cnt=10000;//初始化，防止一开始当TIM5->CNT未变时，出现误差。
int16_t init_journey_flag=1;
int16_t Journey_direction;


//注：行程开关不要用外部中断（会出现检测不到触发边的情况！！！）
///**
// * @brief  外部中断配置函数(行程开关)
// * @param  None
// * @retval None
// */
//void EXTI_Configuration_LimitSwitch(void)
//{
//    /*
//    可能用到的外部中断有：
//        编码器的机械零位相输入
//        两侧行程开关的触发
//    */
//	GPIO_InitTypeDef gpioInit; 
//	EXTI_InitTypeDef extiInit;
//	NVIC_InitTypeDef nvicInit;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	
//	gpioInit.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_13; 
//	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
//	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &gpioInit); 
//  
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3|GPIO_PinSource13);  
//  extiInit.EXTI_Line = EXTI_Line3|EXTI_Line13; 
//  extiInit.EXTI_Mode = EXTI_Mode_Interrupt; 
//  extiInit.EXTI_Trigger = EXTI_Trigger_Falling; 
//  extiInit.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&extiInit); 
//  EXTI_GenerateSWInterrupt(EXTI_Line3|EXTI_Line13); 
//	
//	nvicInit.NVIC_IRQChannel = EXTI3_IRQn;
//	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
//	nvicInit.NVIC_IRQChannelSubPriority = 1;
//	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
//  NVIC_Init(&nvicInit);
//  nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
//	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
//	nvicInit.NVIC_IRQChannelSubPriority = 1;
//	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
//	NVIC_Init(&nvicInit);
//}



int32_t test_exti;
//		block_disconnect_t* temp = Get_block_t();
///**
// * @brief  外部中断(行程开关触发)服务函数（PB3触发的外部中断）
// * @param  None
// * @retval None
// */
//void EXTI3_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line3) != RESET) 
//	{
//        test_exti++;
//		block_disconnect.is_left_limit = 1;
//		EXTI_ClearFlag(EXTI_Line3);
//		//EXTI_ClearITPendingBit(EXTI_Line3);	
//	}
//}


/**
 * @brief  外部中断（编码器的过零）服务函数(PA10输入的外部中断)
 *         （排除）外部中断（行程开关触发）服务函数(PB13输入的外部中断)
 * @param  None
 * @retval None
 */
extern int32_t roundNum;//记录编码器转过了几次机械零位
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET) 
	{//编码器的过零触发 PA10输入
        test_exti++;
        roundNum++;
		EXTI_ClearFlag(EXTI_Line10);
		//EXTI_ClearITPendingBit(EXTI_Line10);	//这俩函数的效果是一样的
	}
    
//    if(EXTI_GetITStatus(EXTI_Line13) != RESET) 
//	{//行程开关触发 PB13输入
//        test_exti++;
//		block_disconnect.is_right_limit = 1;
//		EXTI_ClearFlag(EXTI_Line13);
//		//EXTI_ClearITPendingBit(EXTI_Line13);	//这俩函数的效果是一样的
//	}
}
