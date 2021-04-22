#include "main.h"
#include "HW_USART1_debugPC.h"

// 尝试用这个串口给电脑输出数据
// USART1用来定时像电脑发送CPU使用率的信息
// 这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

// 因为USART2好像没引出来，所以暂时还是用USART1试一波裁判系统
extern uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];    //从裁判系统接收的数据
extern uint8_t JudgeSendBuffer[JudgeSendBufSize];          //向裁判系统发出的数据

/**
  * @brief  USART1配置
  * @param  None
  * @retval None
  */
void USART1_Configuration(void)
{
    USART_InitTypeDef uartInit;
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);

    uartInit.USART_BaudRate = 115200;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &uartInit);

    USART_Cmd(USART1, ENABLE);
    
    USART_ClearFlag(USART1, USART_FLAG_TXE|USART_FLAG_TC|USART_FLAG_IDLE);
    
    USART_ITConfig(USART1,USART_IT_TC|USART_IT_IDLE,ENABLE);
    
    NVIC_InitTypeDef nvicInit;
    nvicInit.NVIC_IRQChannel = USART1_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
    
	{
		DMA_InitTypeDef  dmaInit;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Channel5);
		
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
		dmaInit.DMA_MemoryBaseAddr = (uint32_t)JudgeReceiveBuffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
		dmaInit.DMA_BufferSize = JudgeBufBiggestSize;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular;//原先是DMA_Mode_Circular;，但我的理解用Normal模式更合理吧？
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel5, &dmaInit);
		DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);   //个人感觉，这个中断开归开，但不要在这个里面处理接收的数据会比较好
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		nvicInit.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
	}
}


//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{   
    USART_SendData(USART1, (uint8_t)ch);  // 发送一个字节数据到串口
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// 等待发送完毕
    return (ch);
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// 等待串口输入数据
    return (int)USART_ReceiveData(USART1);
}



/**
  * @brief  USART1中断服务函数(接收裁判系统的各种数据)
  * @param  None
  * @retval None
  */
uint8_t Judge_Decode_Flag =0 ;
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(USART1,USART_IT_TC);
    }
    
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        //一旦进入了空闲中断，就说明一帧数据包传完了，
        //但由于不定长，所以DMA的接收完成中断时不一定会发生的，
        //而且在这个空闲中断里面，还要把DMA的缓存清空，缓存指针置回原位
        //没有配置双缓冲模式的原因不是因为不会，而是考虑到裁判系统的通信的频率较低，
        //处理速度不是瓶颈，没必要采取用空间换时间的策略
        
        USART_ClearITPendingBit(USART1,USART_IT_IDLE); //清除中断标志位
        (void)USART1->DR; //清空数据寄存器（读一遍之后，自动清除IDLE）
        
        DMA_Cmd(DMA1_Channel5, DISABLE); //先关DMA
  //      Judge_Receive(JudgeReceiveBuffer);  //处理从裁判系统接收的数据
        Judge_Decode_Flag = SET;
        //之所以不在这个中断里面直接处理Judge_Receive(),是因为处理这个函数时间太长，会报硬错误HardFault（但也不是每次都会报）
        //所以这里只相当于给一个标志位，然后另外开一个任务来处理（目前合并到了闪灯任务里了）
        
        DMA_SetCurrDataCounter(DMA1_Channel5, JudgeBufBiggestSize);  //重新装填待传输的字节数（不过我感觉 循环模式下，不手动装填应该也行吧？）
        DMA_Cmd(DMA1_Channel5, ENABLE);  //重开DMA 
    }
}


/**
  * @brief  DMA接收中断
  * @param  None
  * @retval None
  */
//void DMA1_Channel5_IRQHandler(void)
//{
//	if(DMA_GetFlagStatus(DMA1_FLAG_TC5) != RESET)
//	{
//		// JudgeBuffReceive(JudgeReceiveBuffer);
//		DMA_ClearFlag(DMA1_FLAG_TC5);
//	}
//}


/**
  * @brief  发送一个字节 
  * @param  USART外设地址
  *         待发送的字符 
  * @retval none
  */
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);  //发送一个字节数据到USART->TDR   
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//等待发送数据寄存器TDR清空   
}


/**
  * @brief  发送8位的数组  
  * @param  USART外设地址
  *         待发送的数组头地址
  *         数组中元素的个数
  * @retval none
  */
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
//    DMA_Cmd(DMA1_Stream4,DISABLE);
//    DMA_SetCurrDataCounter(DMA1_Stream4, num);
//    DMA_Cmd(DMA1_Stream4,ENABLE);
    for (uint16_t i = 0; i < num; i++)
    {
        USART_SendByte(pUSARTx, *(array+i)); //放一个byte到USART->TDR
    }
    
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// 等待发送完成
//    DMA_Cmd(DMA1_Stream4,DISABLE);
}


///**
//  * @brief  发送字符串
//  * @param  USART外设地址
//  *         字符串头地址
//  * @retval none
//  */
//void USART_SendString(USART_TypeDef *pUSARTx, char *str)
//{
//    for(uint16_t k = 0;*(str+k) != '\0';k++)
//        USART_SendByte(pUSARTx, *(str + k));

//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;  // 等待发送完成
//}

///**
//  * @brief  发送一个16位数
//  * @param  USART外设地址
//  *         16位数
//  * @retval none
//  */
//void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    temp_h = (ch & 0XFF00) >> 8; /* 取出高八位 */
//    temp_l = ch & 0XFF;   /* 取出低八位 */

//    USART_SendByte(pUSARTx, temp_h); /* 发送高八位 */
//    USART_SendByte(pUSARTx, temp_l);  /* 发送低八位 */
//}





