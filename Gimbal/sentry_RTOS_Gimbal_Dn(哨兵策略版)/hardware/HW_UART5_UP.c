///**************************************************************************
//* author : Hellocoded
//* time   : 2022/2/10
//* function:ͨ������5������̨ͨ��,ʵ�ָ���ʶ����˵�Ч��
//****************************************************************************/


//#include "main.h"
//#include "HW_UART5_UP.h"


//uint8_t Send_Up_buffer[Send_Up_Size] = {0,0,0};
//uint8_t Rec_Up_buffer[Rec_Up_Size];

///*************************************
//* ����5��ͨ������
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
//        dmaInit.DMA_Memory0BaseAddr = (uint32_t)Rec_Up_buffer;
//        dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
//        dmaInit.DMA_BufferSize = Rec_Up_Size;
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
//        dmaInit.DMA_Memory0BaseAddr = (uint32_t)Send_Up_buffer;
//        dmaInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//        dmaInit.DMA_BufferSize = Send_Up_Size;
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
//  * @brief  ����5 DMA�����ж�
//  * @param  None
//  * @retval None
//  */
//void DMA1_Stream7_IRQHandler(void)
//{
//    if (DMA_GetFlagStatus(DMA1_Stream7, DMA_IT_TCIF7) == SET)
//    {
//        DMA_Cmd(DMA1_Stream7, DISABLE);
//        DMA_SetCurrDataCounter(DMA1_Stream7, Send_Up_Size);
//        DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
//        DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
//    }
//}

//uint8_t tempUp[Rec_Up_Size]; //����Ҫ��Ϊȫ�ֱ�������Ȼcrc��ͨ��
//int16_t Up_Crcpass, Up_crcNopass;
////PC_Recv_t PC_Recv_D;
//uint8_t Up_ErrorBuff[Rec_Up_Size * 4];
//int16_t Up_buffindex;
///**
//  * @brief  ����2 DMA�����ж�
//  * @param  None
//  * @retval None
//  */
//void DMA1_Stream0_IRQHandler(void)
//{
//    static uint8_t temptemp[2 * Rec_Up_Size];
//    int16_t PackPoint, n;
//    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
//    {
//        DMA_Cmd(DMA1_Stream0,DISABLE);
//        memcpy(temptemp + PC_RECVBUF_SIZE, Rec_Up_buffer, Rec_Up_Size);
//        for (PackPoint = 0; PackPoint < Rec_Up_Size; PackPoint++) //��ֹ��λ����һ������Ԫ�صĵ�һ����Ϊ
//        {
//            if (temptemp[PackPoint] == '!')
//            {
//                for (n = 0; n < Rec_Up_Size; n++)
//                {
//                    tempUp[n] = temptemp[(n + PackPoint)];
//                }
//                Up_crcNopass++;
//                if (Verify_CRC8_Check_Sum(tempUp, Rec_Up_Size))
//                    Up_Receive(tempUp);
//                else
//                {
//									  Up_buffindex++;
//                    Up_buffindex = Up_buffindex % 4;
//                }
//                break;
//            }
//        }
//        DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
//        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
//        memcpy(temptemp, temptemp + Rec_Up_Size, Rec_Up_Size);
//        DMA_Cmd(DMA1_Stream0,ENABLE);
//    }
//}


//float robot_length; //������������̨֮�����ֱ����
//float k_angle; //tan(up)��ֵ
//void Angle_Trans(void)
//{
//	
//   extern Up_Receive_t Up_Receive_Now;
//   
//	 k_angle = (robot_length/Up_Receive_Now.Up_distance - sinf(Up_Receive_Now.Up_pitch)) \
//	            / (cosf(Up_Receive_Now.Up_yaw));
//	
//	 Up_Receive_Now.Down_Trans_yaw = acosf(sqrtf(1/(k_angle * k_angle + 1)));
//	 Up_Receive_Now.Down_Trans_pitch = asinf(k_angle * sqrtf(1/(k_angle * k_angle + 1)));


//}


///**************************************************************
//* function��������̨������׼���ĽǶ�ֵ
//*
//***************************************************************/
//short send_pitch;
//int   send_yaw;
//void Down_UART5Send(void)
//{
//   //extern Down_Receive_t Down_Receive_Now;
//	 extern float aim_yaw, aim_pitch;
//   extern uint8_t Send_Up_buffer[Send_Up_Size];
//	 extern uint8_t last_armor_state;
//	 extern uint8_t armor_state;
//	
//	 send_pitch = (short)(aim_pitch * 100);
//	 send_yaw = (int)(aim_yaw * 100);
//	
//	
//	 Send_Up_buffer[0] = '!';
//	 Send_Up_buffer[1] = (unsigned char)(send_pitch >> 8 & 0x00FF);
//   Send_Up_buffer[2] = (unsigned char) (send_pitch & 0x00FF);
//	
//	 Send_Up_buffer[3] = (unsigned char)(send_yaw >> 24 & 0x000000FF);
//	 Send_Up_buffer[4] = (unsigned char)(send_yaw >> 16 & 0x000000FF);
//	 Send_Up_buffer[5] = (unsigned char)(send_yaw >> 8 & 0x000000FF);
//	 Send_Up_buffer[6] = (unsigned char)(send_yaw >> 0 & 0x000000FF);
//	 Send_Up_buffer[7] = (unsigned char) (last_armor_state==ARMOR_NO_AIM && armor_state == ARMOR_AIMED);//��׼״̬������
//	 
//   Append_CRC8_Check_Sum(Send_Up_buffer, Send_Up_Size);
//   DMA_Cmd(DMA1_Stream7,ENABLE);

//}


