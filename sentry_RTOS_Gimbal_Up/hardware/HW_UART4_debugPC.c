#include "main.h"
#include "HW_UART4_debugPC.h"

//尝试用这个串口给电脑输出数据
//把UART4用来定时向电脑发送CPU使用率的信息
//这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

/**
  * @brief  UART4配置
  * @param  None
  * @retval None
  */
void UART4_Configuration(void)
{
    USART_InitTypeDef uartInit;
    GPIO_InitTypeDef gpioInit;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);

    gpioInit.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
    gpioInit.GPIO_Mode = GPIO_Mode_AF;
    gpioInit.GPIO_OType = GPIO_OType_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &gpioInit);

    uartInit.USART_BaudRate = 115200;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx/*|USART_Mode_Rx*/;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4, &uartInit);

    USART_Cmd(UART4, ENABLE);
    
    USART_ClearFlag(UART4, USART_FLAG_TXE);
    
 NVIC_InitTypeDef nvicInit;
 nvicInit.NVIC_IRQChannel = UART4_IRQn;
 nvicInit.NVIC_IRQChannelPreemptionPriority = 3;
 nvicInit.NVIC_IRQChannelSubPriority = 0;
 nvicInit.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&nvicInit);

    //USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
    //{
    //    DMA_InitTypeDef dmaInit;
    //    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    //    DMA_DeInit(DMA1_Stream4);
    //    dmaInit.DMA_Channel = DMA_Channel_4;
    //    dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (UART4->DR);
    //    dmaInit.DMA_Memory0BaseAddr = (uint32_t); //这里放存cpu状态的那一堆字符串
    //    dmaInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    //    dmaInit.DMA_BufferSize = 0;//看需要串数数据的时候，用多少设多少
    //    dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //    dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //    dmaInit.DMA_Mode = DMA_Mode_Normal; //这里用普通模式 只在printf调用的时候才开DMA的传输通道
    //    dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
    //    dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
    //    dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    //    dmaInit.DMA_MemoryBurst = DMA_Mode_Normal;
    //    dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    //    DMA_Init(DMA1_Stream4, &dmaInit);
    //    //DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE); //不清楚为什么要开DMA的传输完成中断了
    //    DMA_Cmd(DMA1_Stream4, ENABLE);
    //}
}






//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{   
    USART_SendData(UART4, (uint8_t)ch);  // 发送一个字节数据到串口
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET) ;// 等待发送完毕
    return (ch);
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET) ;// 等待串口输入数据
    return (int)USART_ReceiveData(UART4);
}




/**
  * @brief  UART4中断服务函数
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
    if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(UART4,USART_IT_TC);
    }
}


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





