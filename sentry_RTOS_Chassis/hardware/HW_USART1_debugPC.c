#include "main.h"
#include "HW_USART1_debugPC.h"

// ������������ڸ������������
// USART1������ʱ����Է���CPUʹ���ʵ���Ϣ
// ����Ҫ�� printf �� scanf ɶ����һ����װ �������Ժ��ô�������������ʽ�ĵ��ԣ�

// ��ΪUSART2����û��������������ʱ������USART1��һ������ϵͳ
extern uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];    //�Ӳ���ϵͳ���յ�����
extern uint8_t JudgeSendBuffer[JudgeSendBufSize];          //�����ϵͳ����������

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
		dmaInit.DMA_Mode = DMA_Mode_Circular;//ԭ����DMA_Mode_Circular;�����ҵ������Normalģʽ������ɣ�
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel5, &dmaInit);
		DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);   //���˸о�������жϿ��鿪������Ҫ��������洦����յ����ݻ�ȽϺ�
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		nvicInit.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
	}
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



/**
  * @brief  USART1�жϷ�����(���ղ���ϵͳ�ĸ�������)
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
        //һ�������˿����жϣ���˵��һ֡���ݰ������ˣ�
        //�����ڲ�����������DMA�Ľ�������ж�ʱ��һ���ᷢ���ģ�
        //��������������ж����棬��Ҫ��DMA�Ļ�����գ�����ָ���û�ԭλ
        //û������˫����ģʽ��ԭ������Ϊ���ᣬ���ǿ��ǵ�����ϵͳ��ͨ�ŵ�Ƶ�ʽϵͣ�
        //�����ٶȲ���ƿ����û��Ҫ��ȡ�ÿռ任ʱ��Ĳ���
        
        USART_ClearITPendingBit(USART1,USART_IT_IDLE); //����жϱ�־λ
        (void)USART1->DR; //������ݼĴ�������һ��֮���Զ����IDLE��
        
        DMA_Cmd(DMA1_Channel5, DISABLE); //�ȹ�DMA
  //      Judge_Receive(JudgeReceiveBuffer);  //����Ӳ���ϵͳ���յ�����
        Judge_Decode_Flag = SET;
        //֮���Բ�������ж�����ֱ�Ӵ���Judge_Receive(),����Ϊ�����������ʱ��̫�����ᱨӲ����HardFault����Ҳ����ÿ�ζ��ᱨ��
        //��������ֻ�൱�ڸ�һ����־λ��Ȼ�����⿪һ������������Ŀǰ�ϲ����������������ˣ�
        
        DMA_SetCurrDataCounter(DMA1_Channel5, JudgeBufBiggestSize);  //����װ���������ֽ����������Ҹо� ѭ��ģʽ�£����ֶ�װ��Ӧ��Ҳ�аɣ���
        DMA_Cmd(DMA1_Channel5, ENABLE);  //�ؿ�DMA 
    }
}


/**
  * @brief  DMA�����ж�
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
  * @brief  ����һ���ֽ� 
  * @param  USART�����ַ
  *         �����͵��ַ� 
  * @retval none
  */
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);  //����һ���ֽ����ݵ�USART->TDR   
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//�ȴ��������ݼĴ���TDR���   
}


/**
  * @brief  ����8λ������  
  * @param  USART�����ַ
  *         �����͵�����ͷ��ַ
  *         ������Ԫ�صĸ���
  * @retval none
  */
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
//    DMA_Cmd(DMA1_Stream4,DISABLE);
//    DMA_SetCurrDataCounter(DMA1_Stream4, num);
//    DMA_Cmd(DMA1_Stream4,ENABLE);
    for (uint16_t i = 0; i < num; i++)
    {
        USART_SendByte(pUSARTx, *(array+i)); //��һ��byte��USART->TDR
    }
    
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// �ȴ��������
//    DMA_Cmd(DMA1_Stream4,DISABLE);
}


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





