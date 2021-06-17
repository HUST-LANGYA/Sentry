#include "main.h"
#include "HW_TIM3_Encoder.h"

_encoder_t encoder;//编码器

void TIM3_Config_Encoder(void) //A、B相
{
    GPIO_InitTypeDef gpioInit;
    TIM_TimeBaseInitTypeDef timTimeBase;
    TIM_ICInitTypeDef timICInit;
    NVIC_InitTypeDef nvicInit;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    timTimeBase.TIM_Prescaler = 0x0;
    timTimeBase.TIM_Period = 65535;
    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &timTimeBase);
    
    GPIO_StructInit(&gpioInit);
    gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //分别为A、B相
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    TIM_DeInit(TIM3);

    TIM_ICStructInit(&timICInit);
    timICInit.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // 配置输入捕获通道12
    timICInit.TIM_ICFilter = 0x00;                         // IC1F=0011",IC2F="0011" 输入捕获1和2的滤波器?
    timICInit.TIM_ICPolarity = TIM_ICPolarity_Rising;      // 上升沿触发
    timICInit.TIM_ICSelection = TIM_ICSelection_DirectTI;  // CC1S="01",CC2S="02",IC1FP1映射到TI1,IC2FP2=TI2
    timICInit.TIM_ICPrescaler = TIM_ICPSC_DIV1;            //无预分频器，捕获输入口上检测到的每一个边沿都触发一次捕获
    TIM_ICInit(TIM3, &timICInit);
    
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    //这一个函数干了3件事情（写了CCMR1的CC1S和CC2S位，写了SMCR的SMS位，写了CCER的CC1E,CC1P,CC2E,CC2P位）：
    //CC1通道被配置为输入，IC1映射在TI1上；
    //CC2通道被配置为输入，IC2映射在TI2上；
    //配置两个捕获通道均不反相，在上升沿捕获；
    //使用编码器模式3 – 根据另一个信号的输入电平，计数器在TI1FP1和TI2FP2的边沿向上/下计数。 

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);      //清除“更新中断”、“触发中断”的标志位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能 更新中断、触发中断

    nvicInit.NVIC_IRQChannel = TIM3_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 3;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    TIM_SetCounter(TIM3, 0x00); //复位TIM3的计数器（CNT寄存器）
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief  单位时间读取编码器计数
 * @param  None
 * @retval 编码器计数
 */

/*inline*/ uint16_t Read_Encoder(void)
{
   // int16_t encoderVal;
   // encoderVal = TIM_GetCounter(TIM3); //读取计数器的值
   //// TIM_SetCounter(TIM3, 0);           //计数器的值清零
   // return encoderVal;
   
    uint16_t encoderVal = TIM_GetCounter(TIM3);  //读编码器计数值
    return encoderVal;
}

void TIM3_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
    {
        //记一次溢出，判断上溢还是下溢，然后记录period的圈数
        //注意：这个圈数不是编码器转过的圈数
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    }
}
