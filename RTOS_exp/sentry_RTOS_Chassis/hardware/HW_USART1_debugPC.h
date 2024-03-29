#ifndef __HW_USART1_debugPC_H
#define __HW_USART1_debugPC_H

//尝试用这个串口给电脑输出数据
//把UART4用来定时向电脑发送CPU使用率的信息
//这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

#define USART1_TX_MAX_BUFFER  0xff  //127

void USART1_Configuration(void);
void USART_SendArray(USART_TypeDef* pUSARTx, uint8_t* array, uint16_t num); //发送8位的数组  (没开DMA的版本)

#endif //__HW_USART1_debugPC_H
