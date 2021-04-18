#include "main.h"
#include "HW_USART1_debugPC.h"

//������������ڸ������������
//USART1������ʱ����Է���CPUʹ���ʵ���Ϣ
//����Ҫ�� printf �� scanf ɶ����һ����װ �������Ժ��ô�������������ʽ�ĵ��ԣ�

/**
  * @brief  USART1����
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






//�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{   
    USART_SendData(USART1, (uint8_t)ch);  // ����һ���ֽ����ݵ�����
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// �ȴ��������
    return (ch);
}

//�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// �ȴ�������������
    return (int)USART_ReceiveData(USART1);
}




///**
//  * @brief  USART1�жϷ�����
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





