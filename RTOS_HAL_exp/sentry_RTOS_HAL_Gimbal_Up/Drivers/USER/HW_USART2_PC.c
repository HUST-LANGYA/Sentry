#include "main.h"
#include "HW_USART2_PC.h"

uint8_t PCbuffer[PC_RECVBUF_SIZE] = {0, 0, 0};
uint8_t PC_SendBuf[PC_SENDBUF_SIZE];

/* 要按照hal库的标准，换成回调函数！！*/
// 先不管这个外设吧，好像开路DMA之后，那些关于接收和发送的回调函数都在DMA里头实现的了，不是在串口的全局中断里面实现的

//我这里对串口2的移植绝壁是错的，等要上PC的时候再改吧，实在是想不清楚这一堆乱七八糟的回调时怎么调的
/**
  * @brief  串口2空闲中断配置
  * @param  None
  * @retval None
  */
//uint8_t ucTemp[8];
//void USART2_IRQHandler_IDLE_CallBack(void)  //这里以后要把名字改成对应的 空闲中断的回调函数的名字
//{
//	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
//	{
//		(void)USART2->SR; //clear the IDLE int    USART_SR_IDLE
//		(void)USART2->DR;
//	}
//    
//    
//   
////	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
////	{	for(int i=0;i<8;i++)
////        {        
////            ucTemp[i] = USART_ReceiveData(USART2);
////           // while(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET);
////        }
////        PCReceive(ucTemp);  
////	}	
//}


uint8_t tempPC[PC_RECVBUF_SIZE]; //这里要改为全局变量，不然crc不通过
int16_t Crcpass, crcNopass;
PC_Recv_t PC_Recv_D;
uint8_t ErrorBuff[PC_RECVBUF_SIZE * 4];
int16_t buffindex;


/**
  * @brief  处理串口2接收的数据的回调函数（可能是在空闲中断里执行？）
  * @param  None
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart2)
    {
        static uint8_t temptemp[2 * PC_RECVBUF_SIZE];
        int16_t PackPoint, n;
        if (DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
        {
            memcpy(temptemp + PC_RECVBUF_SIZE, PCbuffer, PC_RECVBUF_SIZE);
            for (PackPoint = 0; PackPoint < PC_RECVBUF_SIZE; PackPoint++) //防止错位，不一定数组元素的第一个就为
            {
                if (temptemp[PackPoint] == '!')
                {
                    for (n = 0; n < PC_RECVBUF_SIZE; n++)
                    {
                        tempPC[n] = temptemp[(n + PackPoint)];
                    }
                    crcNopass++;
                    if (Verify_CRC8_Check_Sum(tempPC + 1, PC_RECVBUF_SIZE - 2))
                    {   extern uint32_t tick2,tick1;
                        tick2=xTaskGetTickCountFromISR()-tick1;
                        PCReceive(tempPC);
                        //extern uint32_t tick1;
                        tick1=xTaskGetTickCountFromISR();
                    }
                    else
                    {
                        buffindex++;
                        buffindex = buffindex % 4;
                    }
                    break;
                }
            }
            //DMA_Cmd(DMA1_Stream5,DISABLE);
            DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
            DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
            memcpy(temptemp, temptemp + PC_RECVBUF_SIZE, PC_RECVBUF_SIZE);
        }
    }
}

/**
  * @brief  串口2 DMA发送中断
  * @param  None
  * @retval None
  */
//void DMA1_Stream6_IRQHandler(void)
//{
//	if (DMA_GetFlagStatus(DMA1_Stream6, DMA_IT_TCIF6) == SET)
//	{
//		DMA_Cmd(DMA1_Stream6, DISABLE);
//		DMA_SetCurrDataCounter(DMA1_Stream6, PC_SEND_BUF_SIZE);
//		DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
//		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
//	}
//}
