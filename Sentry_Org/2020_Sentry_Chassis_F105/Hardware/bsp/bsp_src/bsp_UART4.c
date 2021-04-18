#include "main.h"

uint8_t GYRO0_buffer[GYRO_BUF_SIZE];
void USART4_Configuration(void)
{
	USART_InitTypeDef usartInit;
	GPIO_InitTypeDef  gpioInit;
	NVIC_InitTypeDef  nvicInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	gpioInit.GPIO_Pin = GPIO_Pin_11;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_10;  
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC	,&gpioInit);

	USART_DeInit(UART4);
	usartInit.USART_BaudRate = 115200;
	usartInit.USART_WordLength = USART_WordLength_8b;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_Parity = USART_Parity_No ;
	usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_Init(UART4,&usartInit);
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);   //开启串口闲时中断  
	USART_Cmd(UART4,ENABLE);
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); 
	
	nvicInit.NVIC_IRQChannel = UART4_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInit.NVIC_IRQChannelSubPriority = 1;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&nvicInit);
	
	{
		DMA_InitTypeDef  dmaInit;
		
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1|RCC_AHBPeriph_DMA2, ENABLE);
		DMA_DeInit(DMA2_Channel3);
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(UART4->DR);
		dmaInit.DMA_MemoryBaseAddr = (uint32_t)GYRO0_buffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
		dmaInit.DMA_BufferSize = JudgeBufBiggestSize;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular;
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;
		
		nvicInit.NVIC_IRQChannel = DMA2_Channel3_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
		nvicInit.NVIC_IRQChannelSubPriority = 1;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);
		
		DMA_Init(DMA2_Channel3, &dmaInit);
		DMA_ITConfig(DMA2_Channel3,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA2_Channel3, ENABLE);
	}
}

void DMA2_Channel3_IRQHandler(void)
{	
	if(DMA_GetFlagStatus(DMA2_FLAG_TC3) == SET)
	{
		DMA_ClearFlag(DMA2_FLAG_TC3);
	}
}

void UART4_IRQHandler(void)
{
	if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
	{

		(void)UART4->SR;   //clear the IDLE int32_t
		(void)UART4->DR;	
		
		int16_t n,i;
		uint8_t temp[GYRO_BUF_SIZE];
		static int32_t DATA_LENGTH2=0;
		DMA_Cmd(DMA2_Channel3, DISABLE);
		DATA_LENGTH2=20-DMA_GetCurrDataCounter(DMA2_Channel3);
		if(DATA_LENGTH2==GYRO_BUF_SIZE)
		{
			for(n=0;n<GYRO_BUF_SIZE;n++)
			{
				if(GYRO0_buffer[n] == 0x55)
				{
					for(i=0;i<GYRO_BUF_SIZE;i++)
					{
						temp[i]=GYRO0_buffer[(n+i)%GYRO_BUF_SIZE];
					}
					if(Verify_CRC8_Check_Sum(temp, GYRO_BUF_SIZE) == 1)
					{
//						block_disconnect_t* block_disconnect =  Get_block_t();
//						block_disconnect->Gyro_Last_Cnt = GetSysCnt();
//						GyroReceiveFunc(GYRO0_buffer,n);
					}
					break;
				}
			} 
	  }
		DMA_SetCurrDataCounter(DMA2_Channel3,20);	
		DMA_Cmd(DMA2_Channel3,ENABLE);
	}
}

