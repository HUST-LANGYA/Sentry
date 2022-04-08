/*****************************************************************************
* 时   间：2021年8月7号
* 文件内容:USART4的初始化及中断服务函数、和上位机的通信部分
* 作   者:Hellocoded
*****************************************************************************/

#include "main.h"

unsigned char GYRO_Buffer[GYRO_BUF_SIZE];
Gyro_Typedef Mpu6050Receive;
uint8_t Offet_Send[OFFET_SEND_SIZE]; //发送给上位机的数组
uint8_t Offet_Rece[OFFET_RECE_SIZE];
//// 接收分解来自上位机的数据
//uint8_t Speedx[2] = {0,0};
//uint8_t Speedy[2] = {0,0};
//uint8_t Speedw[2] = {0,0};
//float Speedx_Debug = 0.0f;
//float Speedy_Debug = 0.0f;
//float Speedw_Debug = 0.0f;

//解析来自手机的数据
//uint8_t PC_Cmd_Flag = 0;
uint8_t PC_Cmd = 0;

extern PID_Typedef Chassis_WheelSpeed[4];
extern ChassisSpeed_t Chassis_Speed; //三轴速度值
void USART4_Init(void)
{
	USART_InitTypeDef usart;
	GPIO_InitTypeDef  gpio;
	NVIC_InitTypeDef  nvic;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
  //RX  PC11  
	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC,&gpio);
  //TX  PC10
	gpio.GPIO_Pin = GPIO_Pin_10;  
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC	,&gpio);
  //Usart4 Init   
	USART_DeInit(UART4);
	usart.USART_BaudRate = 115200;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No ;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_Init(UART4,&usart);
	//Idle Time Interrupt
	//USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
  USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
	//USART_ITConfig(UART4,USART_IT_TC,ENABLE);
	USART_ClearFlag(UART4,USART_IT_RXNE);
	USART_Cmd(UART4,ENABLE);
	//USART_DMACmd(UART4, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);  //易错//先都开启
	
	nvic.NVIC_IRQChannel = UART4_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&nvic);
	 //发送DMA
//	{
//		DMA_InitTypeDef  dma;
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
// 		DMA_DeInit(DMA2_Channel5);
//		dma.DMA_PeripheralBaseAddr = (uint32_t)&(UART4->DR);
//		dma.DMA_MemoryBaseAddr = (uint32_t)Offet_Send;
//		dma.DMA_DIR = DMA_DIR_PeripheralDST;/*DMA_DIR_PeripheralSRC;*/
//		dma.DMA_BufferSize = OFFET_RECE_SIZE;
//		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
//		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//		dma.DMA_Mode = DMA_Mode_Circular; //循环发送
//		dma.DMA_Priority = DMA_Priority_VeryHigh;
//		dma.DMA_M2M = DMA_M2M_Disable;
//		
//		nvic.NVIC_IRQChannel = DMA2_Channel5_IRQn;
//		nvic.NVIC_IRQChannelPreemptionPriority = 1;
//		nvic.NVIC_IRQChannelSubPriority = 1;
//		nvic.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvic);
//		
//		DMA_Init(DMA2_Channel5, &dma);
//	  DMA_ITConfig(DMA2_Channel5,DMA_IT_TC,ENABLE);
//		DMA_ClearFlag(DMA2_FLAG_TC5);
//		DMA_Cmd(DMA2_Channel5, ENABLE);
//	}
//	 //接收DMA
//	{
//	  DMA_InitTypeDef  dma;
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
// 		DMA_DeInit(DMA2_Channel3);
//		dma.DMA_PeripheralBaseAddr = (uint32_t)&(UART4->DR);
//		dma.DMA_MemoryBaseAddr = (uint32_t)Offet_Rece;
//		dma.DMA_DIR = DMA_DIR_PeripheralSRC;
//		dma.DMA_BufferSize = OFFET_RECE_SIZE;
//		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
//		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//		dma.DMA_Mode = DMA_Mode_Circular; //循环发送
//		dma.DMA_Priority = DMA_Priority_VeryHigh;
//		dma.DMA_M2M = DMA_M2M_Disable;
//		
//		nvic.NVIC_IRQChannel = DMA2_Channel3_IRQn;
//		nvic.NVIC_IRQChannelPreemptionPriority = 1;
//		nvic.NVIC_IRQChannelSubPriority = 1;
//		nvic.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&nvic);
//		
//		DMA_Init(DMA2_Channel3, &dma);
//		DMA_ITConfig(DMA2_Channel3,DMA_IT_TC,ENABLE);
//		DMA_Cmd(DMA2_Channel3, ENABLE);  
//	
//	
//	}
	
}


/******************************************************
* 函数作用：DMA发送中断
*
******************************************************/


//void DMA2_Channel5_IRQHandler(void)
//{	
//	if(DMA_GetFlagStatus(DMA2_FLAG_TC5) != RESET)
//	{
//		
//		DMA_Cmd(DMA2_Channel5,DISABLE);
//		DMA_SetCurrDataCounter(DMA2_Channel5,OFFET_SEND_SIZE);
//		DMA_Cmd(DMA2_Channel5,ENABLE);
//		DMA_ClearFlag(DMA2_FLAG_TC5);
//		
//	}
//}


///******************************************************
//* 函数作用: DMA接收中断
//*
//******************************************************/
//void DMA2_Channel3_IRQHandler(void)
//{
//   if(DMA_GetFlagStatus(DMA2_FLAG_TC3) == SET)
//	 {
//		  DMA_Cmd(DMA2_Channel3,DISABLE);
//		  DMA_SetCurrDataCounter(DMA2_Channel3,OFFET_SEND_SIZE);
//	    DMA_ClearFlag(DMA2_FLAG_TC5);
//	 }

//}



/*******************************************************
* 函数作用：上位机数据接收串口中断
*
********************************************************/

void UART4_IRQHandler(void)
{
   if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)
	 {
	    (void) UART4->SR;
	    (void) UART4->DR;
		 // DMA_Cmd(DMA2_Channel3,DISABLE);
      PC_Cmd = USART_ReceiveData(UART4); //接收CMD
		  if(PC_Cmd=='R')
			{
				Offet_Check();
				Offet_Check_Send(); //以数组的形式发送，不行的话就以字符串的形式发送
			  //PC_Cmd_Flag = 1;
				PC_Cmd = 0;
				
			}
			
			//SP_Receive(Offet_Rece); //上位机数据处理函数 
			
		  //DMA_SetCurrDataCounter(DMA2_Channel3,OFFET_RECE_SIZE);
		  //DMA_Cmd(DMA2_Channel3,ENABLE);
			USART_ReceiveData(UART4);
		  USART_ClearITPendingBit(UART4,USART_IT_RXNE);
	    
	 }


}


void Usart_SendByte(USART_TypeDef *pUSARTx,uint8_t ch)
{
   USART_SendData(pUSARTx,ch);
   while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}


void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

void Usart_SendString(USART_TypeDef*pUSARTx,char* str)
{
  unsigned int k=0;
do
{
   Usart_SendByte(pUSARTx,*(str+k));
   k++;
}while(*(str+k)!='\0');

 while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}

}

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;//把下8位清零
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(UART4, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(UART4);
}


//short i=0;
//short SP_Rece_Flag = 0;
//void SP_Receive(uint8_t data[])
//{
//   memcpy(Speedx,data,2);
//	 memcpy(Speedy,&data[2],2);
//	 memcpy(Speedw,&data[4],2);
//	
//	 Speedx_Debug = 0.0f;
//	 Speedy_Debug = 0.0f;
//	 Speedw_Debug = 0.0f;
//	 float w[2] = {100.0f,10.0f};
//   //上位机发送轮胎的速度设定值
//	 for(i=0;i<2;i++)
//	 {
//		Speedx[i] = Speedx[i]-48; //全部转化成真实的数据
//		Speedy[i] = Speedy[i]-48;
//		Speedw[i] = Speedw[i]-48;
//		 
//	 }
//	
//	 for(int y=0;y<2;y++)
//	 {
//	  Speedx_Debug += ((float)Speedx[y]*w[y]);
//		Speedy_Debug += ((float)Speedy[y]*w[y]);
//		Speedw_Debug += ((float)Speedw[y]*w[y]);
//	 }
//	 SP_Rece_Flag = 1;
//   //Chassis_WheelSpeed[0].SetPoint = LIMIT_MAX_MIN((float)Speed_Debug,Chassis_WheelSpeedMax,-Chassis_WheelSpeedMax);
////	 Chassis_Speed.CarSpeedx = Speedx_Debug;
////	 Chassis_Speed.CarSpeedy = Speedy_Debug;
////	 Chassis_Speed.CarSpeedw = Speedw_Debug;
//////	 //清零防止累加
////   Speedx_Debug = 0;
////	 Speedy_Debug = 0;
////	 Speedw_Debug = 0;

//}
//void UART4_IRQHandler(void)
//{
//	if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
//	{

//		(void)UART4->SR;   //Clear the IDLE int
//		(void)UART4->DR;	
//		
//		short n;
//		static int DATA_LENGTH0=0;
//		DMA_Cmd(DMA2_Channel3, DISABLE);
//		DATA_LENGTH0=13-DMA_GetCurrDataCounter(DMA2_Channel3);  //current data length in buffer
//		                                                                                                       
//		if(DATA_LENGTH0==GYRO_BUF_SIZE)
//		{
//			for(n=0;n<GYRO_BUF_SIZE;n++)
//			{
//				if(GYRO_Buffer[n] == 0x55)
//				{
//					Mpu6050ReceiveFunc(GYRO_Buffer,n);//Decoding
//				}
//			} 
////			DMA_Cmd(DMA2_Channel3,ENABLE);             //感觉这里的判断有重合的地方
////			DMA_SetCurrDataCounter(DMA2_Channel3,13);	
//	  }
////		else
////		{
//			DMA_Cmd(DMA2_Channel3,ENABLE);
//		  DMA_SetCurrDataCounter(DMA2_Channel3,13);	
////		}
//		//DMA_Cmd(DMA2_Channel3,ENABLE);
//	}
//}


/**************************************************************
* 函数名称:MPU6050Receive
* 函数功能:处理mpu6050发来的数据
* 返 回 值:无
**************************************************************/

//extern volatile uint32_t CPU_Runtime;
//extern Com_offet_t com_offet;
//void Mpu6050ReceiveFunc(unsigned char gyroBuffer[],short BufferNum)  //BufferNum为帧头的位置
//{
//	Mpu6050Receive.LastYawAngle = Mpu6050Receive.ReceiveYawAngle;
//	switch(gyroBuffer[(BufferNum+1)])
//	{	
//		case 0x53://Angle Buff
//		Mpu6050Receive.ReceiveYawAngle = ((short)(gyroBuffer[(BufferNum+7)%GYRO_BUF_SIZE]<<8 | gyroBuffer[(BufferNum+6)%GYRO_BUF_SIZE]))/32767.0*180;//得到YAWangle
//		Mpu6050Receive.Temp = ((short)(gyroBuffer[(BufferNum+9)%GYRO_BUF_SIZE]<<8 | gyroBuffer[(BufferNum+8)%GYRO_BUF_SIZE]))/340.0+36.25;
//		break;
//	}
//	/*Zero_Check*/
//	if(Mpu6050Receive.LastYawAngle-Mpu6050Receive.ReceiveYawAngle>100)   //检查是否转过一圈了,
//	 Mpu6050Receive.ZeroCircle++;
//	if(Mpu6050Receive.LastYawAngle-Mpu6050Receive.ReceiveYawAngle<-100)
//	 Mpu6050Receive.ZeroCircle--;
//  Mpu6050Receive.YawAngle = Mpu6050Receive.ReceiveYawAngle + Mpu6050Receive.ZeroCircle*180.0;
//	
//	//中断次数判断部分
//	com_offet.Uart_offet.Uart_gyro_Cur_Num++;
//	com_offet.Uart_offet.Uart_gyro_Last_Num = com_offet.Uart_offet.Uart_gyro_Cur_Num;
//	
////	com_offet.Uart_offet.Uart_gyro_Last_time = com_offet.Uart_offet.Uart_gyro_Cur_time;
////	com_offet.Uart_offet.Uart_gyro_Cur_time = CPU_RunTime;
////	
////	if((com_offet.Uart_offet.Uart_gyro_Cur_time-com_offet.Uart_offet.Uart_gyro_Last_time)>100)
////	   com_offet.Uart_offet.Uart_gyro_offet = 1;
////	else 
////		 com_offet.Uart_offet.Uart_gyro_offet = 0;
////	
//}

/**
  * @brief  Return ReceiveGyroYawAngle
  * @param 
  * @retval 
  */
float GyroYawAngle(void)
{
	return Mpu6050Receive.YawAngle;
}
