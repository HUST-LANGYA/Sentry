///**************************************************************************
//* author : Hellocoded
//* time   : 2022/2/10
//* function:通过串口5和下云台通信,实现辅助识别敌人的效果
//****************************************************************************/


//#include "main.h"
//#include "HW_UART5_DOWN.h"


//uint8_t Send_Dn_buffer[Send_Dn_Size] = {0,0,0};
//uint8_t Rec_Dn_buffer[Rec_Dn_Size];

///*************************************
//* 串口5的通信配置
//*************************************/
//void UART5_Configuration(void)
//{
//     USART_InitTypeDef usartInit;
//    GPIO_InitTypeDef  gpioInit;
//    NVIC_InitTypeDef  nvicInit;

//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

//    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

//    gpioInit.GPIO_Pin = GPIO_Pin_12;
//    gpioInit.GPIO_Mode = GPIO_Mode_AF;
//    gpioInit.GPIO_OType = GPIO_OType_PP;
//    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
//    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOC, &gpioInit);

//    gpioInit.GPIO_Pin = GPIO_Pin_2;
//    gpioInit.GPIO_Mode = GPIO_Mode_AF;
//    gpioInit.GPIO_OType = GPIO_OType_PP;
//    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
//    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOD, &gpioInit);

////    gpioInit.GPIO_Pin = GPIO_Pin_1;
////    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
////    gpioInit.GPIO_OType = GPIO_OType_PP;
////    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
////    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
////    GPIO_Init(GPIOA, &gpioInit);

//    usartInit.USART_BaudRate = 115200;
//    usartInit.USART_WordLength = USART_WordLength_8b;
//    usartInit.USART_StopBits = USART_StopBits_1;
//    usartInit.USART_Parity = USART_Parity_No;
//    usartInit.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_Init(UART5, &usartInit);

//    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
//    
//    USART_Cmd(UART5, ENABLE);
//    USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);
//    USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);

//    nvicInit.NVIC_IRQChannel = UART5_IRQn;
//    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
//    nvicInit.NVIC_IRQChannelSubPriority = 0;
//    nvicInit.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvicInit);

//    {
//        DMA_InitTypeDef dmaInit;
//        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//        DMA_DeInit(DMA1_Stream0);
//        dmaInit.DMA_Channel = DMA_Channel_4;
//        dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (UART5->DR);
//        dmaInit.DMA_Memory0BaseAddr = (uint32_t)Rec_Dn_buffer;
//        dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
//        dmaInit.DMA_BufferSize = Rec_Dn_Size;
//        dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//        dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
//        dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//        dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//        dmaInit.DMA_Mode = DMA_Mode_Circular;
//        dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
//        dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
//        dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//        dmaInit.DMA_MemoryBurst = DMA_Mode_Normal;
//        dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

//        nvicInit.NVIC_IRQChannel = DMA1_Stream0_IRQn;
//        nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
//        nvicInit.NVIC_IRQChannelSubPriority = 3;
//        nvicInit.NVIC_IRQChannelCmd = ENABLE;
//        NVIC_Init(&nvicInit);

//        DMA_Init(DMA1_Stream0, &dmaInit);
//        DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);
//        DMA_Cmd(DMA1_Stream0, ENABLE);
//    }

//    {
//        DMA_InitTypeDef dmaInit;
//        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//        DMA_DeInit(DMA1_Stream7);
//        dmaInit.DMA_Channel = DMA_Channel_4;
//        dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (UART5->DR);
//        dmaInit.DMA_Memory0BaseAddr = (uint32_t)Send_Dn_buffer;
//        dmaInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//        dmaInit.DMA_BufferSize = Send_Dn_Size;
//        dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//        dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
//        dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//        dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//        dmaInit.DMA_Mode = DMA_Mode_Normal;
//        dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
//        dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
//        dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//        dmaInit.DMA_MemoryBurst = DMA_Mode_Normal;
//        dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

//        nvicInit.NVIC_IRQChannel = DMA1_Stream7_IRQn;
//        nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
//        nvicInit.NVIC_IRQChannelSubPriority = 1;
//        nvicInit.NVIC_IRQChannelCmd = ENABLE;
//        NVIC_Init(&nvicInit);

//        DMA_Init(DMA1_Stream7, &dmaInit);
//        DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);
//        DMA_Cmd(DMA1_Stream7, DISABLE);
//    }




//}



///**
//  * @brief  串口5 DMA发送中断
//  * @param  None
//  * @retval None
//  */
//void DMA1_Stream7_IRQHandler(void)
//{
//    if (DMA_GetFlagStatus(DMA1_Stream7, DMA_IT_TCIF7) == SET)
//    {
//        DMA_Cmd(DMA1_Stream7, DISABLE);
//        DMA_SetCurrDataCounter(DMA1_Stream7, Send_Dn_Size);
//        DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
//        DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
//    }
//}

//uint8_t tempDown[Rec_Dn_Size]; //这里要改为全局变量，不然crc不通过
//int16_t Down_Crcpass, Down_crcNopass;
////PC_Recv_t PC_Recv_D;
//uint8_t Down_ErrorBuff[Rec_Dn_Size * 4];
//int16_t Down_buffindex;
///**
//  * @brief  串口2 DMA接收中断
//  * @param  None
//  * @retval None
//  */
//void DMA1_Stream0_IRQHandler(void)
//{
//    static uint8_t temptemp[2 * Rec_Dn_Size];
//    int16_t PackPoint, n;
//    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
//    {
//        DMA_Cmd(DMA1_Stream0,DISABLE);
//        memcpy(temptemp + PC_RECVBUF_SIZE, Rec_Dn_buffer, Rec_Dn_Size);
//        for (PackPoint = 0; PackPoint < Rec_Dn_Size; PackPoint++) //防止错位，不一定数组元素的第一个就为
//        {
//            if (temptemp[PackPoint] == '!')
//            {
//                for (n = 0; n < Rec_Dn_Size; n++)
//                {
//                    tempDown[n] = temptemp[(n + PackPoint)];
//                }
//                Down_crcNopass++;
//                if (Verify_CRC8_Check_Sum(tempDown, Rec_Dn_Size))
//                    Down_Receive(tempDown);
//                else
//                {
//                    Down_buffindex++;
//                    Down_buffindex = Down_buffindex % 4;
//                }
//                break;
//            }
//        }
//        DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
//        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
//        memcpy(temptemp, temptemp + Rec_Dn_Size, Rec_Dn_Size);
//        DMA_Cmd(DMA1_Stream0,ENABLE);
//    }
//}


//float robot_length; //机器人上下云台之间的竖直距离
//float k_angle; //tan(up)的值
//void Angle_Trans(void)
//{
//	
//   extern Down_Receive_t Down_Receive_Now;
//   
//	 k_angle = (robot_length/Down_Receive_Now.Down_distance - sinf(Down_Receive_Now.Down_pitch)) \
//	            / (cosf(Down_Receive_Now.Down_yaw));
//	
//	// Down_Receive_Now.Up_Trans_yaw = acosf(sqrtf(1/(k_angle * k_angle + 1)));
//	 Down_Receive_Now.Up_Trans_yaw = Down_Receive_Now.Down_yaw;//直接采用相同的YAW，使得两个云台在同一平面内
//	 Down_Receive_Now.Up_Trans_pitch = asinf(k_angle * sqrtf(1/(k_angle * k_angle + 1)));


//}



