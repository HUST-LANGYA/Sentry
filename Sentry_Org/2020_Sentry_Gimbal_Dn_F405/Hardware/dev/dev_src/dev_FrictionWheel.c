#include "main.h"
//uint32_t SRC_Buffer[] = {12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23, 12, 23};

///**
//  * @brief  设置摩擦轮电机
//  * @param  None
//  * @retval None
//  */
//void FrictionWheel_Configration(void)
//{
//	GPIO_InitTypeDef gpioInit;
//	TIM_OCInitTypeDef timOCInit;
//	TIM_TimeBaseInitTypeDef timTimeBase;
//	DMA_InitTypeDef dmaInit;
//	NVIC_InitTypeDef nvicInit;

//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

//	gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB10  TIM2_ch3
//	gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PC8 Tim8--Ch3     PC9 Tim8--Ch4
//	gpioInit.GPIO_Mode = GPIO_Mode_AF;
//	gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
//	gpioInit.GPIO_OType = GPIO_OType_PP;
//	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOC, &gpioInit);

//	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
//	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
//	
//	DMA_DeInit(DMA2_Stream1); //tim8_up
//	dmaInit.DMA_Channel = DMA_Channel_7;
//	dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (TIM8->DMAR);
//	dmaInit.DMA_Memory0BaseAddr = (uint32_t)SRC_Buffer;
//	dmaInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	dmaInit.DMA_BufferSize = 32;
//	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
//	dmaInit.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;
//	dmaInit.DMA_Mode = DMA_Mode_Circular;
//	dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
//	dmaInit.DMA_FIFOMode = DMA_FIFOMode_Enable;
//	dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	dmaInit.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream1, &dmaInit);

//	timTimeBase.TIM_Prescaler = 8 - 1; //21Mhz
//	timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
//	timTimeBase.TIM_Period = 35 - 1; //600k  1.67us    12   23
//	timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInit(TIM8, &timTimeBase);

//	timOCInit.TIM_OCMode = TIM_OCMode_PWM2;
//	timOCInit.TIM_OutputState = TIM_OutputState_Enable;
//	timOCInit.TIM_Pulse = 12;
//	timOCInit.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OC1Init(TIM8, &timOCInit);
//	timOCInit.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OC2Init(TIM8, &timOCInit);

//	nvicInit.NVIC_IRQChannel = DMA2_Stream1_IRQn;
//	nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
//	nvicInit.NVIC_IRQChannelSubPriority = 2;
//	nvicInit.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&nvicInit);

//	TIM_DMAConfig(TIM8, TIM_DMABase_CCR1, TIM_DMABurstLength_2Transfers);
//	TIM_DMACmd(TIM8, TIM_DMA_Update, ENABLE);
//	TIM_Cmd(TIM8, ENABLE);
//	TIM_CtrlPWMOutputs(TIM8, ENABLE);
//	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
//	DMA_Cmd(DMA2_Stream1, ENABLE);
//}

///**
//  * @brief  DMA传输完成中断
//  * @param  None
//  * @retval None
//  */
//void DMA2_Stream1_IRQHandler(void)
//{
//	if (DMA_GetFlagStatus(DMA2_Stream1, DMA_IT_TCIF1) == SET)
//	{
//		TIM_DMACmd(TIM8, TIM_DMA_Update, DISABLE);
//		DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, DISABLE);
//		DMA_Cmd(DMA2_Stream1, DISABLE);
//		TIM_SetCompare1(TIM8, 0);
//		TIM_SetCompare2(TIM8, 0);
//		TIM_Cmd(TIM4, ENABLE);
//		TIM_ClearFlag(TIM4, TIM_FLAG_Update);
//		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
//	}
//}

/**
  * @brief  设置摩擦轮转速（油门值）
  * @param  两轮速度值
  * @retval None
  */
extern uint16_t accelerator0;
extern uint16_t accelerator1;
//void FrictionWheel_Set(uint16_t speed0, uint16_t speed1)
//{

//	// speed0 = LIMIT_MAX_MIN(speed0, 2000, 0); //snail
//	// speed1 = LIMIT_MAX_MIN(speed1, 2000, 0); //snail
//	// accelerator0 = speed0;
//	// accelerator1 = speed1;

//	accelerator0 = LIMIT_MAX_MIN(speed0, 2000, 0);
//	accelerator1 = LIMIT_MAX_MIN(speed1, 2000, 0);
//	
//	//	speed=LIMIT_MAX_MIN(speed,2000,1000);//snail
//	//	TIM_SetCompare3(TIM2,speed);
//	//	TIM_SetCompare4(TIM2,speed);
//}


extern uint16_t throttle;
void FrictionWheel_Set(uint16_t tmpThrottle)
{
    throttle=LIMIT_MAX_MIN(tmpThrottle,2000,0);
}

