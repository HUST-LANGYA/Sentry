#include "main.h"
#include "HW_USART3_RC.h"

volatile uint8_t sbus_rx_buffer[2][RX_USART3_BUFFER];  //尝试双缓冲模式

/* 要按照hal库的标准，换成回调函数！！*/
/**
  * @brief  USART3空闲中断的回调函数
  * @param  None
  * @retval None
  */
void USER_UART_RxIDLECallback(UART_HandleTypeDef *huart)
{
        __HAL_UART_CLEAR_FLAG(&huart3,USART_FLAG_IDLE);//清空中断标志位
        (void)USART3->DR; //清空数据寄存器，等待下一次接收

        if(__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) == RX_USART3_BUFFER)     //ensure received complete frame data. 
        {
//// 但是啊！ hal库好像给双缓冲的接口很不全啊！！！所以这里先作罢了，等我啥时候想清楚hal库的设计模式再回来写这个吧，回头拿标准库用了还是
            RemoteReceive(DMA_GetCurrentMemoryTarget(&hdma_usart3_rx)?sbus_rx_buffer[0]:sbus_rx_buffer[1]);
        }
        else 
        {//这个地方是可能收到了残缺帧，故从下一帧开始重新收完整帧
            __HAL_DMA_DISABLE(&hdma_usart3_rx);
            __HAL_DMA_SET_COUNTER(&hdma_usart3_rx,RX_USART3_BUFFER);  
            __HAL_DMA_ENABLE(&hdma_usart3_rx);
        }
}
