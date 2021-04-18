#ifndef __BSP_USART2_H
#define __BSP_USART2_H
#define GYRO_BUF_SIZE 18
#define PC_SENDBUF_SIZE 8
#define PC_RECVBUF_SIZE 8

typedef struct
{
	float RCPitch;
	float RCYaw;
	int16_t ReceiveFromTx2BullectCnt;
	int16_t FrictionWheel_speed;
	int16_t DisConnect;
}PC_Recv_t;

void USART2_Configuration(void);

#endif //__BSP_USART2_H
