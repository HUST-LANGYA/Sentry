#include "main.h"
int32_t Tim2_usCnt = 0;
/**
  * @brief  TIM2初始化
  * @param  None
  * @retval None
  */
void TIM2_Config(void)
{
//    TIM_TimeBaseInitTypeDef timTimeBase;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
//    timTimeBase.TIM_Prescaler = 7 - 1;
//    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
//    timTimeBase.TIM_Period = 12 - 1;

//    TIM_TimeBaseInit(TIM2, &timTimeBase);
//    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    
    TIM_TimeBaseInitTypeDef timTimeBase;
    NVIC_InitTypeDef nvicInit;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    timTimeBase.TIM_Prescaler = 0;
    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
    timTimeBase.TIM_RepetitionCounter = 0;
    timTimeBase.TIM_Period = 65535; //1ms

    nvicInit.NVIC_IRQChannel = TIM2_IRQn;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInit);

    TIM_TimeBaseInit(TIM2, &timTimeBase);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Tim2_usCnt++;
  }
}

int32_t tim = 0;
void delay_us(int32_t t)
{
//    TIM_SetCounter(TIM2, 0);
//    TIM_Cmd(TIM2, ENABLE);

//    while (t--)
//    {
//        while (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != SET);
//        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//    }

//    TIM_Cmd(TIM2, DISABLE);
    
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);

    while (tim < (168 * t))
        tim = TIM_GetCounter(TIM2);

    TIM_Cmd(TIM2, DISABLE);
    
}

//void delay_ms(int32_t t)  //这个毫秒有问题目前
//{
//    //TIM_Cmd(TIM2, ENABLE);
//    while (t--) delay_us(1000);
//    //TIM_Cmd(TIM2, DISABLE);
//}


