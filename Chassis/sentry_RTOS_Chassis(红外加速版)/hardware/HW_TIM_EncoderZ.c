#include "main.h"
#include "HW_TIM_EncoderZ.h"

//尚未测试该底层是否时正确的

extern block_disconnect_t block_disconnect;

/**
 * @brief  编码器Z相读取(编码器过零触发)
 * @param  None
 * @retval None
 */
int16_t EncoderZ;  //Z相触发计数
void TIM1_Config_EncoderZ(void)
{     
	GPIO_InitTypeDef gpioInit;
	NVIC_InitTypeDef nvicInit;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef timTimeBase;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    timTimeBase.TIM_Prescaler = 0x0;
    timTimeBase.TIM_Period = 65535;
    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &timTimeBase);
    
    GPIO_StructInit(&gpioInit);
	gpioInit.GPIO_Pin = GPIO_Pin_10; 
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit); 
    
    TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;

	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	
	nvicInit.NVIC_IRQChannel = TIM1_UP_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority =  3;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&nvicInit);
    
    TIM_ClearFlag(TIM1, TIM_FLAG_Update/* | TIM_FLAG_Trigger*/);      //清除“更新中断”、“触发中断”的标志位
    TIM_ITConfig(TIM1, TIM_IT_Update/* | TIM_FLAG_Trigger*/, ENABLE); //使能 更新中断、触发中断
    
    TIM_SetCounter(TIM1, 0x00); //复位TIM1的计数器（CNT寄存器）
    TIM_Cmd(TIM1, ENABLE);
}

/*inline*/ int16_t Read_Encoder_Z(void)
{    
    EncoderZ =GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10) ;
    int16_t encoderZVal = TIM_GetCounter(TIM1);  //读编码器计数值

    return encoderZVal;
}

void TIM1_UP_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM1,TIM_IT_Update) != RESET)
    {
  //////////////////////////////////////////
        TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
    }
}
