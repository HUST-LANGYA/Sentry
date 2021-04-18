#include "main.h"
#include "HW_USART1_debugPC.h"

//尝试用这个串口给电脑输出数据
//USART1用来定时像电脑发送CPU使用率的信息
//这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

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
    uartInit.USART_Mode = USART_Mode_Tx/*|USART_Mode_Rx*/;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &uartInit);

    USART_Cmd(USART1, ENABLE);
    
    USART_ClearFlag(USART1, USART_FLAG_TXE);
    
    // NVIC_InitTypeDef nvicInit;
    // nvicInit.NVIC_IRQChannel = USART1_IRQn;
    // nvicInit.NVIC_IRQChannelPreemptionPriority = 3;
    // nvicInit.NVIC_IRQChannelSubPriority = 0;
    // nvicInit.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&nvicInit);
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




///**
//  * @brief  USART1中断服务函数
//  * @param  None
//  * @retval None
//  */
//void USART1_IRQHandler(void)
//{
//    if (USART_GetITStatus(USART1, USART_IT_TX) != RESET)
//    {
//    }
//}


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





