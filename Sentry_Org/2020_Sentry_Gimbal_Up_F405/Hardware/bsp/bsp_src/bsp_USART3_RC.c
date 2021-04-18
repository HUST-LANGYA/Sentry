#include "main.h"
extern RC_Ctl_t RC_Ctl;
volatile uint8_t sbus_rx_buffer[18];

/**
  * @brief  配置USART3
  * @param  None
  * @retval None
  */
void USART3_Configuration(void)
{
	GPIO_InitTypeDef gpioInit;
	USART_InitTypeDef usartInit;
	NVIC_InitTypeDef nvicInit;

    
//这一块注释掉好像没有关系（因为PC4要用去点灯了，所以这里就注释掉了）
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//	gpioInit.GPIO_Pin = GPIO_Pin_4;
//	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
//	gpioInit.GPIO_OType = GPIO_OType_PP;
//	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &gpioInit);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	gpioInit.GPIO_Pin = GPIO_Pin_11;
	gpioInit.GPIO_Mode = GPIO_Mode_AF;
	gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &gpioInit);


	usartInit.USART_BaudRate = 100000;
	usartInit.USART_WordLength = USART_WordLength_8b;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_Parity = USART_Parity_Even;
	usartInit.USART_Mode = USART_Mode_Rx;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &usartInit);

	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

	nvicInit.NVIC_IRQChannel = USART3_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInit.NVIC_IRQChannelSubPriority = 2;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInit);

	{
		DMA_InitTypeDef dmaInit;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_DeInit(DMA1_Stream1);
		dmaInit.DMA_Channel = DMA_Channel_4;
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DR);
		dmaInit.DMA_Memory0BaseAddr = (uint32_t)sbus_rx_buffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dmaInit.DMA_BufferSize = RX_USART3_BUFFER;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular;
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
		dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dmaInit.DMA_MemoryBurst = DMA_Mode_Normal;
		dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream1, &dmaInit);
		DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Stream1, ENABLE);
	}
}
/**
  * @brief  USART3空闲中断
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	static int16_t DATA_LENGTH = 0;
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		(void)USART3->SR;
		(void)USART3->DR;
		DMA_Cmd(DMA1_Stream1, DISABLE);
		DATA_LENGTH = RX_USART3_BUFFER - DMA_GetCurrDataCounter(DMA1_Stream1);
		if (DATA_LENGTH == 18)
			RemoteReceive();
		{
			//RemoteReceive(sbus_rx_buffer);
		}
		DMA_SetCurrDataCounter(DMA1_Stream1, RX_USART3_BUFFER);
		DMA_Cmd(DMA1_Stream1, ENABLE);
	}
}
