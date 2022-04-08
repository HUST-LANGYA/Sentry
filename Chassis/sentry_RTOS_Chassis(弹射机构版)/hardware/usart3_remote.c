/*************************************************************
* 时   间：2021年8月7号
* 文件内容：USART3的初始化、与遥控器之间的通信
* 作    者：Hellocoded
*************************************************************/

#include "main.h"

extern RC_Ctl_t RC_Ctl;
volatile unsigned char uart3_RxBuffer[18];

void USART3_Init(void)
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
	 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);   //开启串口闲时中断 
	USART_Cmd(USART3,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); 

	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
		
	{
		DMA_InitTypeDef  dma;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
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
	}
}
/**
  * @brief  DMA接收中断
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
/**
  * @brief  串口的接收中断
  * @param  None
  * @retval None
  */

//extern volatile uint32_t CPU_Runtime;
//extern Com_offet_t com_offet;
void USART3_IRQHandler(void)
{
	static int DATA_LENGTH=0;
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		(void)USART3->SR;
		(void)USART3->DR;	
		DMA_Cmd(DMA1_Channel3,DISABLE);
		DATA_LENGTH = RX_USART3_BUFFER - DMA_GetCurrDataCounter(DMA1_Channel3);
		if(DATA_LENGTH==18)
			RemoteReceive(uart3_RxBuffer);
		
//		com_offet.Uart_offet.Uart_remote_Last_time = com_offet.Uart_offet.Uart_remote_Cur_time;
//		com_offet.Uart_offet.Uart_remote_Cur_time = CPU_RunTime;
//		
//		if((com_offet.Uart_offet.Uart_remote_Cur_time-com_offet.Uart_offet.Uart_remote_Last_time)>100)
//		   com_offet.Uart_offet.Uart_remote_offet = 1;
//		else
//		   com_offet.Uart_offet.Uart_remote_offet = 0;
		
		
		
		DMA_SetCurrDataCounter(DMA1_Channel3,RX_USART3_BUFFER);	
		DMA_Cmd(DMA1_Channel3,ENABLE);
  }
}



/********************************************************************
* 函数名称:RemoteReceive
* 函数作用:对遥控器的数据进行解码
* 
*********************************************************************/
short test_flag= 0;
void RemoteReceive(volatile unsigned char rx_buffer[])
{	
	test_flag = !test_flag;
	RC_Ctl.rc.ch0 = (rx_buffer[0]| (rx_buffer[1] << 8)) & 0x07ff; 
	RC_Ctl.rc.ch1 = ((rx_buffer[1] >> 3) | (rx_buffer[2] << 5)) & 0x07ff; 
	RC_Ctl.rc.ch2 = ((rx_buffer[2] >> 6) | (rx_buffer[3] << 2) | (rx_buffer[4] << 10)) & 0x07ff;
	RC_Ctl.rc.ch3 = ((rx_buffer[4] >> 1) | (rx_buffer[5] << 7)) & 0x07ff; 
	RC_Ctl.rc.s1 = ((rx_buffer[5] >> 4)& 0x0003); 
	RC_Ctl.rc.s2 = ((rx_buffer[5] >> 6)& 0x0003);
}


/*********************************************************************
* 函数名称：Reset_RcData
* 函数作用：对接收的遥控器数据进行初始化清零
*
*********************************************************************/

void Reset_RcData(void)
{
   RC_Ctl.rc.ch0 = 1024;
	 RC_Ctl.rc.ch1 = 1024;
	 RC_Ctl.rc.ch2 = 1024;
	 RC_Ctl.rc.ch3 = 1024;
	 
	 RC_Ctl.rc.s1 = 2;
   RC_Ctl.rc.s2 = 2;

}

