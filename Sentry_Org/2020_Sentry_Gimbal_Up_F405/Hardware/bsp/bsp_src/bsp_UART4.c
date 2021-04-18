#include "main.h"

uint8_t GYRO0_buffer[GYRO_BUF_SIZE];

/**
  * @brief  UART4ÅäÖÃ
  * @param  None
  * @retval None
  */
void UART4_Configuration(void)
{
	USART_InitTypeDef uartInit;
	GPIO_InitTypeDef gpioInit;
	NVIC_InitTypeDef nvicInit;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);

	gpioInit.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	gpioInit.GPIO_Mode = GPIO_Mode_AF;
	gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &gpioInit);

	uartInit.USART_BaudRate = 115200;
	uartInit.USART_WordLength = USART_WordLength_8b;
	uartInit.USART_StopBits = USART_StopBits_1;
	uartInit.USART_Parity = USART_Parity_Even;
	uartInit.USART_Mode = USART_Mode_Rx;
	uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART4, &uartInit);

	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	USART_Cmd(UART4, ENABLE);
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);

	nvicInit.NVIC_IRQChannel = UART4_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInit.NVIC_IRQChannelSubPriority = 2;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInit);

	{
		DMA_InitTypeDef dmaInit;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Stream2);
		dmaInit.DMA_Channel = DMA_Channel_4;
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (UART4->DR);
		dmaInit.DMA_Memory0BaseAddr = (uint32_t)GYRO0_buffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dmaInit.DMA_BufferSize = RX_UART4_BUFFER;
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
		DMA_Init(DMA1_Stream2, &dmaInit);
		DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Stream2, ENABLE);
	}
}

/**
  * @brief  ??UART4?????
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
	static int32_t DATA_LENGTH = 0;
	int16_t n, i;
	uint8_t temp[GYRO_BUF_SIZE];

	if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
	{
		(void)UART4->SR;
		(void)UART4->DR;

		//static int DATA_LENGTH=0;
		DMA_Cmd(DMA1_Stream2, DISABLE);
		DATA_LENGTH = 20 - DMA_GetCurrDataCounter(DMA1_Stream2);
		if (DATA_LENGTH == GYRO_BUF_SIZE)
		{
			for (n = 0; n < GYRO_BUF_SIZE; n++)
			{
				if (GYRO0_buffer[n] == 0x55)
				{
					for (i = 0; i < GYRO_BUF_SIZE; i++)
					{
						temp[i] = GYRO0_buffer[(n + i) % GYRO_BUF_SIZE];
					}
					if (Verify_CRC8_Check_Sum(temp, GYRO_BUF_SIZE) == 1)
					{
						//block_disconnet_t* block_disconnet =  Get_block_t();
						//block_disconnet->Gyro_Last_Cnt = GetSysCnt();
						//GyroReceiveFunc(GYRO0_buffer,n);
					}
					break;
				}
			}
		}
		DMA_SetCurrDataCounter(DMA1_Stream2, 20);
		DMA_Cmd(DMA1_Stream2, ENABLE);
	}
}
