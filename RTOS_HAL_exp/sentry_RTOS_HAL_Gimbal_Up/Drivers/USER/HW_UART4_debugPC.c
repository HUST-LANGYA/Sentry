#include "main.h"
#include "HW_UART4_debugPC.h"

//�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{   
    USART_SendData(UART4, (uint8_t)ch);  // ����һ���ֽ����ݵ�����
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET) ;// �ȴ��������
    return (ch);
}

//�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET) ;// �ȴ�������������
    return (int)USART_ReceiveData(UART4);
}




///**
//  * @brief  UART4�жϷ�����
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
//  * @brief  ����һ���ֽ� 
//  * @param  USART�����ַ
//  *         �����͵��ַ� 
//  * @retval none
//  */
//void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
//{
//    USART_SendData(pUSARTx, ch);  //����һ���ֽ����ݵ�USART->TDR   
//    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//�ȴ��������ݼĴ���TDR���   
//}


///**
//  * @brief  ����8λ������  
//  * @param  USART�����ַ
//  *         �����͵�����ͷ��ַ
//  *         ������Ԫ�صĸ���
//  * @retval none
//  */
//void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
//{
////    DMA_Cmd(DMA1_Stream4,DISABLE);
////    DMA_SetCurrDataCounter(DMA1_Stream4, num);
////    DMA_Cmd(DMA1_Stream4,ENABLE);
//    for (uint16_t i = 0; i < num; i++)
//    {
//        USART_SendByte(pUSARTx, *(array+i)); //��һ��byte��USART->TDR
//    }
//    
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// �ȴ��������
//    DMA_Cmd(DMA1_Stream4,DISABLE);
//}


///**
//  * @brief  �����ַ���
//  * @param  USART�����ַ
//  *         �ַ���ͷ��ַ
//  * @retval none
//  */
//void USART_SendString(USART_TypeDef *pUSARTx, char *str)
//{
//    for(uint16_t k = 0;*(str+k) != '\0';k++)
//        USART_SendByte(pUSARTx, *(str + k));

//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;  // �ȴ��������
//}

///**
//  * @brief  ����һ��16λ��
//  * @param  USART�����ַ
//  *         16λ��
//  * @retval none
//  */
//void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    temp_h = (ch & 0XFF00) >> 8; /* ȡ���߰�λ */
//    temp_l = ch & 0XFF;   /* ȡ���Ͱ�λ */

//    USART_SendByte(pUSARTx, temp_h); /* ���͸߰�λ */
//    USART_SendByte(pUSARTx, temp_l);  /* ���͵Ͱ�λ */
//}





