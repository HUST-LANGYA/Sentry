#ifndef __BSP_USART2_PC_H
#define __BSP_USART2_PC_H

#define GYRO_BUF_SIZE 18
#define PC_SENDBUF_SIZE 10
#define PC_RECVBUF_SIZE 10

#define PC_SEND_OTHER 0x11
#define PC_SEND_GYRO 0x10
#define PC_SEND_POWER_DOWN 0x03

#define PC_RECEIVR_BUF_SIZE 10
#define PC_SEND_BUF_SIZE	10

typedef struct
{
	float RCPitch;
	float RCYaw;
	int16_t ReceiveFromTx2BulletCnt;
	int16_t FrictionWheel_speed;
	int16_t DisConnect;
}PC_Recv_t;

void USART2_Configuration(void);

#endif //__BSP_USART2_PC_H
