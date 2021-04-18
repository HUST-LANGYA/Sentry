#ifndef __TASK_DATA_SEND_H
#define __TASK_DATA_SEND_H

//下云台发送报文的ID  
#define SLAVE_ID 0x705    
#define BLUE_ID 0x707

#define GYRO_BUF_SIZE 18
#define PC_SENDBUF_SIZE 8
#define PC_RECVBUF_SIZE 8

#define PC_SEND_OTHER 0x11
#define PC_SEND_GYRO 0x10
#define PC_SEND_POWER_DOWN 0x03

#define PC_RECEIVR_BUF_SIZE 8
#define PC_SEND_BUF_SIZE	8

void Gyro_Can2Send(int16_t a,int16_t b);
void Bodan_Can1Send(int16_t c);
void SlaveMsgSend(void);
void PC_MsgSend(unsigned char State);

#endif //__TASK_DATA_SEND_H

