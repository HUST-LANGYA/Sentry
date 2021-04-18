#include "main.h"
#include "HW_UART4_debugPC.h"

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




///**
//  * @brief  UART4中断服务函数
//  * @param  None
//  * @retval None
//  */
//void UART4_IRQHandler(void)
//{
//    if (USART_GetITStatus(UART4, USART_IT_TX) != RESET)
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





