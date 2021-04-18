#include "main.h"

uint8_t uart3_RxBuffer[18];
void USART3_Configuration(void)
{
	USART_InitTypeDef usart;
	GPIO_InitTypeDef  gpio;
	NVIC_InitTypeDef  nvic;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_11;				//PB11 RX
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_10;  		//PB10 TX
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);

	USART_DeInit(USART3);
	usart.USART_BaudRate = 100000;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_Even ;
	usart.USART_Mode = USART_Mode_Rx;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_Init(USART3,&usart);
	 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);   //����������ʱ�ж� 
	USART_Cmd(USART3,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); 

	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
		
//  //����dma
//	{
//		DMA_InitTypeDef  dma;
//		
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//		
//		DMA_DeInit(DMA1_Channel2);
//		
//		dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
//		dma.DMA_MemoryBaseAddr = (uint32_t)DataScopeSend_Buf;
//		dma.DMA_DIR = DMA_DIR_PeripheralDST;
//		dma.DMA_BufferSize = DataScopeSend_BufSize;
//		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
//		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//		dma.DMA_Mode = DMA_Mode_Normal;
//		dma.DMA_Priority = DMA_Priority_VeryHigh;
//		dma.DMA_M2M = DMA_M2M_Disable;
//		
//		DMA_Init(DMA1_Channel2, &dma);
//		DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
//		DMA_Cmd(DMA1_Channel2, ENABLE);
//		
//		nvic.NVIC_IRQChannel = DMA1_Channel2_IRQn;
//		nvic.NVIC_IRQChannelPreemptionPriority = 1;
//		nvic.NVIC_IRQChannelSubPriority = 2;
//		nvic.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvic);
//}
	//����DMA
	{
		DMA_InitTypeDef  dma;
		
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		
		DMA_DeInit(DMA1_Channel3);
		
		dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
		dma.DMA_MemoryBaseAddr = (uint32_t)uart3_RxBuffer;
		dma.DMA_DIR = DMA_DIR_PeripheralSRC;
		dma.DMA_BufferSize = RX_USART3_BUFFER;
		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dma.DMA_Mode = DMA_Mode_Circular;
		dma.DMA_Priority = DMA_Priority_Medium;
		dma.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel3, &dma);
		DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA1_Channel3, ENABLE);
//		nvic.NVIC_IRQChannel = DMA1_Channel3_IRQn;
//		nvic.NVIC_IRQChannelPreemptionPriority = 1;
//		nvic.NVIC_IRQChannelSubPriority = 3;
//		nvic.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvic);
	}
	
}

/**
  * @brief  DMA�����ж�
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC3) == SET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC3);
	}
}

///**
//  * @brief  DMA�����ж�
//  * @param  None
//  * @retval None
//  */
//void DMA1_Channel2_IRQHandler(void)
//{
//	if(DMA_GetFlagStatus(DMA1_FLAG_TC2) == SET)
//	{
//		DMA_Cmd(DMA1_Channel2, DISABLE);
//		DMA_SetCurrDataCounter(DMA1_Channel2, DataScopeSend_BufSize);
//		DMA_ClearFlag(DMA1_FLAG_TC2);
//	}
//}


int32_t cnt1 = 0,cnt2 = 0;
void USART3_IRQHandler(void)
{
	static int32_t DATA_LENGTH=0;
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		(void)USART3->SR;
		(void)USART3->DR;	
		DMA_Cmd(DMA1_Channel3,DISABLE);
		DATA_LENGTH = RX_USART3_BUFFER - DMA_GetCurrDataCounter(DMA1_Channel3);
		cnt2++;
		if(DATA_LENGTH==18)
		{
			cnt1++;
			RemoteReceive();
		}
		DMA_SetCurrDataCounter(DMA1_Channel3,RX_USART3_BUFFER);	
		DMA_Cmd(DMA1_Channel3,ENABLE);
  }
}