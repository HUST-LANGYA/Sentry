#ifndef __BSP_USART3_H
#define __BSP_USART3_H


#define RX_USART3_BUFFER 30

///*ң�����ṹ��*/
//typedef __packed struct
//{
//		unsigned short ch0;
//		unsigned short ch1;
//		unsigned short ch2;
//		unsigned short ch3;
//		unsigned short s1;
//		unsigned short s2;
//}Remote;
///*���ṹ��*/
//typedef __packed 	struct
//{
//		short x;
//		short y;
//		short z;
//		unsigned char press_l;
//		unsigned char press_r;
//}Mouse;
///*���̽ṹ��*/
//typedef __packed struct
//{
//		unsigned short w,s,a,d,q,e,r,f,g,z,x,c,v,b,shift,ctrl;
//}Key;
///*ң����ṹ���ۺ�*/
//typedef __packed struct
//{
//	Remote rc;
//	Mouse mouse;
//	Key key;
//  char RCrecvd,RCDisconnectCnt;//RCrecvdΪ���ݽ��ձ�־λ
//		unsigned char *rx_buffer;
//}RC_Ctl_t;

void USART3_Configuration(void);
void RC_Rst(void);

#endif //__BSP_USART3_H
