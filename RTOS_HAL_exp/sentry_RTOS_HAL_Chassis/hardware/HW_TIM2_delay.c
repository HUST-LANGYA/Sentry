#include "main.h"

/**
  * @brief  TIM2≥ı ºªØ
  * @param  None
  * @retval None
  */
void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef timTimeBase;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    timTimeBase.TIM_Prescaler = 8 - 1;
    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
    timTimeBase.TIM_RepetitionCounter = 0;
    timTimeBase.TIM_Period = 9 - 1;

    TIM_TimeBaseInit(TIM2, &timTimeBase);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

void delay_us(int32_t t)
{
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);

    while (t--)
    {
        while (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != SET);
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    }

    TIM_Cmd(TIM2, DISABLE);
}

void delay_ms(int32_t t)
{
    TIM_Cmd(TIM2, ENABLE);
    while (t--) delay_us(1000);
    TIM_Cmd(TIM2, DISABLE);
}


