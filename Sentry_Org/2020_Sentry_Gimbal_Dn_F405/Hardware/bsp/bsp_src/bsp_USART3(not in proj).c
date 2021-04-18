#include "main.h"
extern RC_Ctl_t RC_Ctl; 
volatile uint8_t sbus_rx_buffer[18];
/**
  * @brief  串口3配置，与遥控器通信
  * @param  None
  * @retval None
  */
void USART3_Configuration(void)
{
	
	GPIO_InitTypeDef  gpioInit;
	USART_InitTypeDef usartInit;
	//GPIO_InitTypeDef  gpioInit;
    NVIC_InitTypeDef  nvicInit;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	gpioInit.GPIO_Pin = GPIO_Pin_4;
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInit);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
	gpioInit.GPIO_Pin = GPIO_Pin_11;
	gpioInit.GPIO_Mode = GPIO_Mode_AF;
    gpioInit.GPIO_OType = GPIO_OType_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&gpioInit);

	usartInit.USART_BaudRate = 100000;
	usartInit.USART_WordLength = USART_WordLength_8b;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_Parity = USART_Parity_Even;
	usartInit.USART_Mode = USART_Mode_Rx;
    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&usartInit);

    USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_Cmd(USART3,ENABLE);
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	

    nvicInit.NVIC_IRQChannel = USART3_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 2;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
		
	{
		DMA_InitTypeDef   dmaInit;
 		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
		DMA_DeInit(DMA1_Stream1);
		dmaInit.DMA_Channel= DMA_Channel_4;
		dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
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
		DMA_Init(DMA1_Stream1,&dmaInit);
		DMA_ITConfig(DMA1_Stream1,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA1_Stream1,ENABLE);
	}
}
/**
  * @brief  串口3 遥控器数据闲时中断
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	static int32_t DATA_LENGTH=0;
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
        (void)USART3->SR;
		(void)USART3->DR;	
        DMA_Cmd(DMA1_Stream1,DISABLE);
        DATA_LENGTH=RX_USART3_BUFFER-DMA_GetCurrDataCounter(DMA1_Stream1);
		if(DATA_LENGTH==18)           
		{
			RemoteReceive();//解码函数
		}
		DMA_SetCurrDataCounter(DMA1_Stream1,RX_USART3_BUFFER);	
		DMA_Cmd(DMA1_Stream1,ENABLE);
    }
}
/**
  * @brief  串口5遥控器数据复位
  * @param  None
  * @retval None
  */
void RC_Rst(void)
{
//		RC_Ctl.rc.ch0 = 1024;
//		RC_Ctl.rc.ch1 = 1024;
//		RC_Ctl.rc.ch2 = 1024;
//		RC_Ctl.rc.ch3 = 1024;
//		RC_Ctl.mouse.x = 0;
//		RC_Ctl.mouse.y = 0;
//		RC_Ctl.mouse.z = 0;
//		RC_Ctl.mouse.press_l = 0;                                                
//		RC_Ctl.mouse.press_r = 0;
//	
//		RC_Ctl.key.w = 0;
//		RC_Ctl.key.s = 0;                            
//		RC_Ctl.key.a = 0;
//		RC_Ctl.key.d = 0;
//		RC_Ctl.key.q = 0;
//		RC_Ctl.key.e = 0;
//		RC_Ctl.key.r = 0;
//		RC_Ctl.key.f = 0;
//		RC_Ctl.key.shift = 0;
//		RC_Ctl.key.ctrl = 0;
//	
//	    RC_Ctl.rc.s1 = 2;
//		RC_Ctl.rc.s2 = 2;
}   
