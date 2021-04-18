#include "main.h"
#include "HW_USART1_debugPC.h"

// ������������ڸ������������
// USART1������ʱ����Է���CPUʹ���ʵ���Ϣ
// ����Ҫ�� printf �� scanf ɶ����һ����װ �������Ժ��ô�������������ʽ�ĵ��ԣ�

// ��ΪUSART2����û��������������ʱ������USART1��һ������ϵͳ
/*extern*/ uint8_t JudgeReceiveBuffer[2][JudgeBufBiggestSize];    //�Ӳ���ϵͳ���յ�����
/*extern*/ uint8_t JudgeSendBuffer[JudgeSendBufSize];          //�����ϵͳ����������

/**
  * @brief  USART1����
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
		dmaInit.DMA_Mode = DMA_Mode_Circular;//ԭ����DMA_Mode_Circular;�����ҵ������Normalģʽ������ɣ�
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
        dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
        dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
        dmaInit.DMA_MemoryBurst =  DMA_MemoryBurst_Single;//DMA_Mode_Normal;
        dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
        //����˫����ģʽ
        DMA_DoubleBufferModeConfig(DMA2_Stream2, (uint32_t)&JudgeReceiveBuffer[1][0], DMA_Memory_0);   //first used memory configuration         
        DMA_DoubleBufferModeCmd(DMA2_Stream2, ENABLE);
        DMA_Init(DMA2_Stream2, &dmaInit);
        //DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);
        DMA_Cmd(DMA2_Stream2, ENABLE);

        //һ�㻺��ģʽ
//		DMA_Init(DMA2_Stream2, &dmaInit);
////		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);   //���˸о�������жϿ��鿪������Ҫ��������洦����յ����ݻ�ȽϺ�
//		DMA_Cmd(DMA2_Stream2, ENABLE);
	}
}


//�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{   
    USART_SendData(USART1, (uint8_t)ch);  // ����һ���ֽ����ݵ�����
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// �ȴ��������
    return (ch);
}

////�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
//int fgetc(FILE *f)
//{
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// �ȴ�������������
//    return (int)USART_ReceiveData(USART1);
//}



/**
  * @brief  USART1�жϷ�����(���ղ���ϵͳ�ĸ�������)
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
//        USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־λ
//        (void)USART1->DR; //������ݼĴ���
//        (void)USART1->SR;
//    }
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        //һ�������˿����жϣ���˵��һ֡���ݰ������ˣ�
        //�����ڲ�����������DMA�Ľ�������ж��ǲ�һ���ᷢ���ģ�
        //��������������ж����棬��Ҫ��DMA�Ļ�����գ�����ָ���û�ԭλ

        (void)USART1->DR; //������ݼĴ�������һ��֮���Զ����IDLE��
//        (void)USART1->SR;
        
        DMA_Cmd(DMA2_Stream2, DISABLE); //�ȹ�DMA
 //       Judge_Receive(JudgeReceiveBuffer);  //����Ӳ���ϵͳ���յ�����
        Judge_Decode_Flag = SET;
        //֮���Բ�������ж�����ֱ�Ӵ���Judge_Receive(),����Ϊ�����������ʱ��̫�����ᱨӲ����HardFault����Ҳ����ÿ�ζ��ᱨ��
        //��������ֻ�൱�ڸ�һ����־λ��Ȼ�����⿪һ������������Ŀǰ�ϲ����������������ˣ�
        
        DMA_SetCurrDataCounter(DMA2_Stream2, JudgeBufBiggestSize);  //����װ���������ֽ���
        DMA_Cmd(DMA2_Stream2, ENABLE);  //�ؿ�DMA
    }
}


/**
  * @brief  DMA�����ж�
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




/////////////////////////////USART����������/////////////////////////
/**
  * @brief  ����һ���ֽ�
  * @param pUSARTx USART�����ַ
  * @param ch      �����͵��ַ�
*/
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);  //����һ���ֽ����ݵ�USART->TDR   
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//�ȴ��������ݼĴ���TDR���   
}


/**
  * @brief  ����8λ������  (û��DMA�İ汾)
  * @param  USART�����ַ
  *         �����͵�����ͷ��ַ
  *         ������Ԫ�صĸ���
  * @retval none
  */
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
    for (uint16_t i = 0; i < num; i++)
    {
        USART_SendByte(pUSARTx, *(array+i)); //��һ��byte��USART->TDR
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// �ȴ��������
}

