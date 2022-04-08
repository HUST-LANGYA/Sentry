#include "main.h"
#include "HW_USART2_judge.h"


extern uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];    //从裁判系统接收的数据
extern uint8_t JudgeSendBuffer[JudgeSendBufSize];          //向裁判系统发出的数据


/**
  * @brief  USART2配置
  * @param  None
  * @retval None
  */
void USART2_Configuration(void)
{
    USART_InitTypeDef uartInit;
    GPIO_InitTypeDef gpioInit;
    NVIC_InitTypeDef nvicInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_2 ;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    
    gpioInit.GPIO_Pin = GPIO_Pin_3;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

//GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_RESET);
//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
//GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);
//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);

    //这些通信的参数需要看着裁判系统的手册改到时候
    uartInit.USART_BaudRate = 115200;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &uartInit);
    
	{
        nvicInit.NVIC_IRQChannel = DMA1_Channel6_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
        
		DMA_InitTypeDef  dmaInit;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Channel6);
		
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
		dmaInit.DMA_MemoryBaseAddr = (uint32_t)JudgeReceiveBuffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
		dmaInit.DMA_BufferSize = JudgeBufBiggestSize;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular;
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel6, &dmaInit);
		DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);   //个人感觉，这个中断开归开，但不要在这个里面处理接收的数据会比较好
		DMA_Cmd(DMA1_Channel6, ENABLE);
	
	}
    
    nvicInit.NVIC_IRQChannel = USART2_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
    
    USART_ClearFlag(USART2, USART_IT_TC);    
    USART_ITConfig(USART2,USART_IT_TC,ENABLE);
    USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
    
    
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收  接受可以提前开启
    USART_Cmd(USART2, ENABLE);
}


/**
  * @brief  USART2中断服务函数(接收裁判系统的各种数据)
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {        
        (void)USART2->SR;
        (void)USART2->DR; //清空数据寄存器
        
        DMA_Cmd(DMA1_Channel6, DISABLE); //先关DMA
        Judge_Receive(JudgeReceiveBuffer);  //处理从裁判系统接收的数据
        DMA_SetCurrDataCounter(DMA1_Channel6, JudgeBufBiggestSize);  //重新装填待传输的字节数（不过我感觉 循环模式下，不手动装填应该也行吧？）
        DMA_Cmd(DMA1_Channel6, ENABLE);  //重开DMA 
    }
}


/**
  * @brief  DMA接收中断
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC6) != RESET)
	{
		Judge_Receive(JudgeReceiveBuffer);
		DMA_ClearFlag(DMA1_FLAG_TC6);
	}
}


///////////////////////////////USART发送驱动库/////////////////////////
///**
//  * @brief  发送一个字节 
//  * @param  USART外设地址
//  *         待发送的字符 
//  * @retval none
//  */
//void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
//{
//    USART_SendData(pUSARTx, ch);  //发送一个字节数据到USART->TDR   
//    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//等待发送数据寄存器TDR清空   
//}


///**
//  * @brief  发送8位的数组  
//  * @param  USART外设地址
//  *         待发送的数组头地址
//  *         数组中元素的个数
//  * @retval none
//  */
//void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
//{
////    DMA_Cmd(DMA1_Stream4,DISABLE);
////    DMA_SetCurrDataCounter(DMA1_Stream4, num);
////    DMA_Cmd(DMA1_Stream4,ENABLE);
//    for (uint16_t i = 0; i < num; i++)
//    {
//        USART_SendByte(pUSARTx, *(array+i)); //放一个byte到USART->TDR
//    }
//    
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// 等待发送完成
//    DMA_Cmd(DMA1_Stream4,DISABLE);
//}


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





