#include "main.h"
#include "HW_USART2_judge.h"

// ! ��һ�����ε����飬������̰�����û������USART2�Ŀ��ӣ����ﻹ������USART1���ð�

uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];    //�Ӳ���ϵͳ���յ�����
uint8_t JudgeSendBuffer[JudgeSendBufSize];          //�����ϵͳ����������


/**
  * @brief  USART2����
  * @param  None
  * @retval None
  */
void USART2_Configuration(void)
{
    USART_InitTypeDef uartInit;
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    //��Щͨ�ŵĲ�����Ҫ���Ų���ϵͳ���ֲ�ĵ�ʱ��
    uartInit.USART_BaudRate = 115200;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &uartInit);

    USART_Cmd(USART2, ENABLE);

    USART_ClearFlag(USART2, USART_FLAG_IDLE);

    NVIC_InitTypeDef nvicInit;
    nvicInit.NVIC_IRQChannel = USART2_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
    
	{
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
		dmaInit.DMA_Mode = DMA_Mode_Normal;//ԭ����DMA_Mode_Circular;�����ҵ������Normalģʽ������ɣ�
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel6, &dmaInit);
		DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);   //���˸о�������жϿ��鿪������Ҫ��������洦����յ����ݻ�ȽϺ�
		DMA_Cmd(DMA1_Channel6, ENABLE);
		
		nvicInit.NVIC_IRQChannel = DMA1_Channel6_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
	}
    
}



/**
  * @brief  USART2�жϷ�����(���ղ���ϵͳ�ĸ�������)
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        //һ�������˿����жϣ���˵��һ֡���ݰ������ˣ�
        //�����ڲ�����������DMA�Ľ�������ж�ʱ��һ���ᷢ���ģ�
        //��������������ж����棬��Ҫ��DMA�Ļ�����գ�����ָ���û�ԭλ
        //û������˫����ģʽ��ԭ������Ϊ���ᣬ���ǿ��ǵ�����ϵͳ��ͨ�ŵ�Ƶ�ʽϵͣ�
        //�����ٶȲ���ƿ����û��Ҫ��ȡ�ÿռ任ʱ��Ĳ���
        
        USART_ClearITPendingBit(USART2,USART_IT_IDLE); //����жϱ�־λ
        (void)USART2->DR; //������ݼĴ���
        
        DMA_Cmd(DMA1_Channel6, DISABLE); //�ȹ�DMA
        Judge_Receive(JudgeReceiveBuffer);  //����Ӳ���ϵͳ���յ�����
        DMA_SetCurrDataCounter(DMA1_Channel6, JudgeBufBiggestSize);  //����װ���������ֽ����������Ҹо� ѭ��ģʽ�£����ֶ�װ��Ӧ��Ҳ�аɣ���
        DMA_Cmd(DMA1_Channel6, ENABLE);  //�ؿ�DMA 
    }
}


/**
  * @brief  DMA�����ж�
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC6) != RESET)
	{
		// JudgeBuffReceive(JudgeReceiveBuffer);
		DMA_ClearFlag(DMA1_FLAG_TC6);
	}
}


///////////////////////////////USART����������/////////////////////////
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





