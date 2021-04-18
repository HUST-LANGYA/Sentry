#include "main.h"

//PA10-USART1_RX
//PA9-USART1_TX

uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];
uint8_t JudgeSendBuffer[JudgeSendBufSize];

void USART1_Configuration(void)
{
	USART_InitTypeDef usartInit;
	GPIO_InitTypeDef  gpioInit;
	NVIC_InitTypeDef  nvicInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	gpioInit.GPIO_Pin = GPIO_Pin_10;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_9;  
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);
	
	USART_DeInit(USART1);
	usartInit.USART_BaudRate = 115200;
	usartInit.USART_WordLength = USART_WordLength_8b;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_Parity = USART_Parity_No ;
	usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_Init(USART1,&usartInit);
	
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);   //开启串口闲时中断  
	USART_Cmd(USART1,ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); 
	
	nvicInit.NVIC_IRQChannel = USART1_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInit);
//  //发送dma
//	{
//		DMA_InitTypeDef  dmaInit;
//		
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//		
//		DMA_DeInit(DMA1_Channel4);
//		
//		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
//		dmaInit.DMA_MemoryBaseAddr = (uint32_t)PC_SendBuf;
//		dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
//		dmaInit.DMA_BufferSize = PC_SEND_BUF_SIZE;
//		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
//		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//		dmaInit.DMA_Mode = DMA_Mode_Normal;
//		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
//		dmaInit.DMA_M2M = DMA_M2M_Disable;
//		
//		DMA_Init(DMA1_Channel4, &dmaInit);
//		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
//		DMA_Cmd(DMA1_Channel4, ENABLE);
//		
//		nvicInit.NVIC_IRQChannel = DMA1_Channel4_IRQn;
//		nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
//		nvicInit.NVIC_IRQChannelSubPriority = 2;
//		nvicInit.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvicInit);
//	}
	//接收DMA
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
		dmaInit.DMA_Mode = DMA_Mode_Circular;
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		DMA_Init(DMA1_Channel5, &dmaInit);
		DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		nvicInit.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
		nvicInit.NVIC_IRQChannelSubPriority = 3;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
	}
}


void USART1_IRQHandler(void)
{   
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		(void)USART1->SR;   //clear the IDLE int
		(void)USART1->DR;	
	}	
}


/**
  * @brief  DMA接收中断
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC5) == SET)
	{
		JudgeBuffReceive(JudgeReceiveBuffer);
		DMA_ClearFlag(DMA1_FLAG_TC5);
	}
}

/**
  * @brief  DMA发送中断
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4) == SET)
	{
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE); 
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel4, PC_SEND_BUF_SIZE);
		DMA_ClearFlag(DMA1_FLAG_TC4);
	}
}


int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t) ch);// USART1
    while (!(USART1->SR & USART_FLAG_TXE));
    return (ch);
}

