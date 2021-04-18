#ifndef __HW_USART2_JUDGE_H
#define __HW_USART2_JUDGE_H
//用这个串口跟裁判系统通信

//#define PC_RECEIVR_BUF_SIZE 8
//#define PC_SEND_BUF_SIZE	8

#ifndef JudgeBufBiggestSize
#define JudgeBufBiggestSize 48  //原先是24，但我感觉如果要接收所有裁判系统的信息，24好像不够长
#endif
#ifndef JudgeSendBufSize
#define JudgeSendBufSize 22
#endif

void USART2_Configuration(void);

#endif //__HW_USART2_JUDGE_H
