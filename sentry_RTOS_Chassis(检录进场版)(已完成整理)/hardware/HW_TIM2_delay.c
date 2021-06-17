#include "main.h"
#include "HW_TIM2_delay.h"

/**
  * @brief  TIM2初始化 1us 一次中断
  * @param  None
  * @retval None
  */
void TIM2_Configuration_Timer(void)
{
  TIM_TimeBaseInitTypeDef timTimeBase;
  NVIC_InitTypeDef nvicInit;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
  timTimeBase.TIM_Prescaler = 0;
  timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
  timTimeBase.TIM_RepetitionCounter = 0;
  timTimeBase.TIM_Period = 65535;

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

int32_t Tim2_usCnt = 0;
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Tim2_usCnt++;
  }
}

/**
  * @brief  Delay
  * @param  None
  * @retval None
  */
void delay_us(int32_t n)
{
  static int32_t tim = 0;
  TIM_SetCounter(TIM2, 0);
  TIM_Cmd(TIM2, ENABLE);

  while (tim < (72 * n))  
    tim = TIM_GetCounter(TIM2);

  TIM_Cmd(TIM2, DISABLE);
}

void delay_ms(int32_t t)
{
    while (t--) delay_us(1000);
}
