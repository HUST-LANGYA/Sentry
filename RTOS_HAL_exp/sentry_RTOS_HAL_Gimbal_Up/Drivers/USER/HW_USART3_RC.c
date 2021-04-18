#include "main.h"
#include "HW_USART3_RC.h"

volatile uint8_t sbus_rx_buffer[2][RX_USART3_BUFFER];  //����˫����ģʽ

/* Ҫ����hal��ı�׼�����ɻص���������*/
/**
  * @brief  USART3�����жϵĻص�����
  * @param  None
  * @retval None
  */
void USER_UART_RxIDLECallback(UART_HandleTypeDef *huart)
{
        __HAL_UART_CLEAR_FLAG(&huart3,USART_FLAG_IDLE);//����жϱ�־λ
        (void)USART3->DR; //������ݼĴ������ȴ���һ�ν���

        if(__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) == RX_USART3_BUFFER)     //ensure received complete frame data. 
        {
//// ���ǰ��� hal������˫����Ľӿںܲ�ȫ�����������������������ˣ�����ɶʱ�������hal������ģʽ�ٻ���д����ɣ���ͷ�ñ�׼�����˻���
            RemoteReceive(DMA_GetCurrentMemoryTarget(&hdma_usart3_rx)?sbus_rx_buffer[0]:sbus_rx_buffer[1]);
        }
        else 
        {//����ط��ǿ����յ��˲�ȱ֡���ʴ���һ֡��ʼ����������֡
            __HAL_DMA_DISABLE(&hdma_usart3_rx);
            __HAL_DMA_SET_COUNTER(&hdma_usart3_rx,RX_USART3_BUFFER);  
            __HAL_DMA_ENABLE(&hdma_usart3_rx);
        }
}
