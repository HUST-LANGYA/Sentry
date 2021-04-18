#include "main.h"
#include "HW_USART2_PC.h"

uint8_t PCbuffer[PC_RECVBUF_SIZE] = {0, 0, 0};
uint8_t PC_SendBuf[PC_SENDBUF_SIZE];

/* Ҫ����hal��ı�׼�����ɻص���������*/
// �Ȳ����������ɣ�����·DMA֮����Щ���ڽ��պͷ��͵Ļص���������DMA��ͷʵ�ֵ��ˣ������ڴ��ڵ�ȫ���ж�����ʵ�ֵ�

//������Դ���2����ֲ�����Ǵ�ģ���Ҫ��PC��ʱ���ٸİɣ�ʵ�����벻�����һ�����߰���Ļص�ʱ��ô����
/**
  * @brief  ����2�����ж�����
  * @param  None
  * @retval None
  */
//uint8_t ucTemp[8];
//void USART2_IRQHandler_IDLE_CallBack(void)  //�����Ժ�Ҫ�����ָĳɶ�Ӧ�� �����жϵĻص�����������
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


uint8_t tempPC[PC_RECVBUF_SIZE]; //����Ҫ��Ϊȫ�ֱ�������Ȼcrc��ͨ��
int16_t Crcpass, crcNopass;
PC_Recv_t PC_Recv_D;
uint8_t ErrorBuff[PC_RECVBUF_SIZE * 4];
int16_t buffindex;


/**
  * @brief  ������2���յ����ݵĻص��������������ڿ����ж���ִ�У���
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
            for (PackPoint = 0; PackPoint < PC_RECVBUF_SIZE; PackPoint++) //��ֹ��λ����һ������Ԫ�صĵ�һ����Ϊ
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
  * @brief  ����2 DMA�����ж�
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
