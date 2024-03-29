#include "main.h"

void TIM2_Configuration(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);      //使能TIM2时钟
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    //时基配置：
        //每32个溢出（即32个period后）产生一个UP中断，
        //在更新中断里面，把下一次的DSHOT的16个数配置好
        //f(HSE)=168MHz,经过预分频器PSC后，得到f(CNT)=24MHz的计时单元，
        //然后以1200个计时单元为一个周期，该周期T=1/f(CNT)*1200=50.0us,
        //看到论坛上面人说是500Hz可以激活C615，这里也先用500Hz试一下先
	TIM_TimeBaseInitStruct.TIM_Prescaler=7-1;                               //时基结构体的预分频数24Mhz(168M/7=24M)
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;              //计数模式（递增计数）
	TIM_TimeBaseInitStruct.TIM_Period=24000-1;                              //f=24MHz/24=1MHz ~ T=1.0us（写到ARR里去）
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;                  //时钟分频数（不分频）
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;                         //重复计数器：写入0到RCR里去（即不重复的意思）
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM2,TIM_FLAG_Update);        //清空中断标志位
}

/**
  * @brief  Millisecond delay (硬延时)
  * @param  None
  * @retval None
  */
void delay_ms(uint32_t t)
{
    TIM_Cmd(TIM2,ENABLE); //使能TIM2
    while(t--)
    {
        while(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)!=SET) ;
        TIM_ClearFlag(TIM2,TIM_FLAG_Update);
    }
    TIM_Cmd(TIM2,DISABLE);
    
    
}
