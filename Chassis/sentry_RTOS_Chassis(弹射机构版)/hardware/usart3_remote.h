#ifndef _USART3_REMOTE_H
#define _USART3_REMOTE_H

#include "main.h"

#define RX_USART3_BUFFER 30


//typedef struct
//{
//  short ch0;
//	short ch1;
//	short ch2;
//	short ch3;
//  short s1;
//	short s2;

//}RC_Ctl_t;



void USART3_Init(void);
void RemoteReceive(volatile unsigned char rx_buffer[]);
void Reset_RcData(void);

#endif
