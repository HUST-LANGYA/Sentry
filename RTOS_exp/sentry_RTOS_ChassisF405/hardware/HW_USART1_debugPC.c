#include "main.h"
#include "HW_USART1_debugPC.h"

// 尝试用这个串口给电脑输出数据
// USART1用来定时像电脑发送CPU使用率的信息
// 这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

// 因为USART2好像没引出来，所以暂时还是用USART1试一波裁判系统
/*extern*/ uint8_t JudgeReceiveBuffer[2][JudgeBufBiggestSize];    //从裁判系统接收的数据
/*extern*/ uint8_t JudgeSendBuffer[JudgeSendBufSize];          //向裁判系统发出的数据

/**
  * @brief  USART1配置
  * @param  None
  * @retval None
  */
void USART1_Configuration(void)
{
    USART_InitTypeDef uartInit;
    GPIO_InitTypeDef gpioInit;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    
    gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpioInit.GPIO_Mode = GPIO_Mode_AF;
    gpioInit.GPIO_OType = GPIO_OType_PP;
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &gpioInit);

    uartInit.USART_BaudRate = 115200;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &uartInit);
    
    USART_ClearFlag(USART1,USART_FLAG_IDLE);
    
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
    USART_Cmd(USART1, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
          
    NVIC_InitTypeDef nvicInit;
    nvicInit.NVIC_IRQChannel = USART1_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
    
	{
        nvicInit.NVIC_IRQChannel = DMA2_Stream2_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
        
		DMA_InitTypeDef  dmaInit;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1|RCC_AHB1Periph_DMA2, ENABLE);
		DMA_DeInit(DMA2_Stream2);
		DMA_Cmd(DMA2_Stream2,DISABLE);
        dmaInit.DMA_Channel = DMA_Channel_4;
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
		dmaInit.DMA_Memory0BaseAddr = (uint32_t)&JudgeReceiveBuffer[0][0];
		dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dmaInit.DMA_BufferSize = JudgeBufBiggestSize;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular;//原先是DMA_Mode_Circular;，但我的理解用Normal模式更合理吧？
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
        dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
        dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
        dmaInit.DMA_MemoryBurst =  DMA_MemoryBurst_Single;//DMA_Mode_Normal;
        dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
        //尝试双缓冲模式
        DMA_DoubleBufferModeConfig(DMA2_Stream2, (uint32_t)&JudgeReceiveBuffer[1][0], DMA_Memory_0);   //first used memory configuration         
        DMA_DoubleBufferModeCmd(DMA2_Stream2, ENABLE);
        DMA_Init(DMA2_Stream2, &dmaInit);
        //DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);
        DMA_Cmd(DMA2_Stream2, ENABLE);

        //一般缓冲模式
//		DMA_Init(DMA2_Stream2, &dmaInit);
////		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);   //个人感觉，这个中断开归开，但不要在这个里面处理接收的数据会比较好
//		DMA_Cmd(DMA2_Stream2, ENABLE);
	}
}


//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{   
    USART_SendData(USART1, (uint8_t)ch);  // 发送一个字节数据到串口
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// 等待发送完毕
    return (ch);
}

////重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
//int fgetc(FILE *f)
//{
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// 等待串口输入数据
//    return (int)USART_ReceiveData(USART1);
//}



/**
  * @brief  USART1中断服务函数(接收裁判系统的各种数据)
  * @param  None
  * @retval None
  */
uint8_t Judge_Decode_Flag =0 ;
void USART1_IRQHandler(void)
{
//    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//    {
//        USART_ClearITPendingBit(USART1,USART_IT_TC);
//    }
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
//        USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志位
//        (void)USART1->DR; //清空数据寄存器
//        (void)USART1->SR;
//    }
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        //一旦进入了空闲中断，就说明一帧数据包传完了，
        //但由于不定长，所以DMA的接收完成中断是不一定会发生的，
        //而且在这个空闲中断里面，还要把DMA的缓存清空，缓存指针置回原位

        (void)USART1->DR; //清空数据寄存器（读一遍之后，自动清除IDLE）
//        (void)USART1->SR;
        
        DMA_Cmd(DMA2_Stream2, DISABLE); //先关DMA
 //       Judge_Receive(JudgeReceiveBuffer);  //处理从裁判系统接收的数据
        Judge_Decode_Flag = SET;
        //之所以不在这个中断里面直接处理Judge_Receive(),是因为处理这个函数时间太长，会报硬错误HardFault（但也不是每次都会报）
        //所以这里只相当于给一个标志位，然后另外开一个任务来处理（目前合并到了闪灯任务里了）
        
        DMA_SetCurrDataCounter(DMA2_Stream2, JudgeBufBiggestSize);  //重新装填待传输的字节数
        DMA_Cmd(DMA2_Stream2, ENABLE);  //重开DMA
    }
}


/**
  * @brief  DMA接收中断
  * @param  None
  * @retval None
  */
//void DMA2_Stream2_IRQHandler(void)
//{
//	if(DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2) != RESET)
//	{
//		// JudgeBuffReceive(JudgeReceiveBuffer);
//		DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);
//	}
//}




/////////////////////////////USART发送驱动库/////////////////////////
/**
  * @brief  发送一个字节
  * @param pUSARTx USART外设地址
  * @param ch      待发送的字符
*/
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);  //发送一个字节数据到USART->TDR   
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//等待发送数据寄存器TDR清空   
}


/**
  * @brief  发送8位的数组  (没开DMA的版本)
  * @param  USART外设地址
  *         待发送的数组头地址
  *         数组中元素的个数
  * @retval none
  */
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
    for (uint16_t i = 0; i < num; i++)
    {
        USART_SendByte(pUSARTx, *(array+i)); //放一个byte到USART->TDR
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// 等待发送完成
}

