#ifndef _USART4_SP_H
#define _USART4_SP_H

#include "main.h"

#define  GYRO_BUF_SIZE 11

//陀螺仪接收结构体
typedef struct
{
	float PitchAngle;
	float YawAngle;
	float ReceiveYawAngle;
	float LastYawAngle;
	short ZeroCircle;
	float Temp;
	ZeroCheck_Typedef Mpu6050_zerocheck;
}Gyro_Typedef;




void USART4_Init(void);
void Usart_SendByte(USART_TypeDef *pUSARTx,uint8_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void SP_Receive(uint8_t data[]);
void Mpu6050ReceiveFunc(unsigned char gyroBuffer[],short BufferNum);
float GyroYawAngle(void);

#endif


