#ifndef __TASK_DATARECEIVE_H
#define __TASK_DATARECEIVE_H

//陀螺仪相关
#define Gyro_Pitch_ID 0x100
#define Gyro_Yaw_ID 0x101

//云台电机相关
#define MotoPitch_ID 0x205 //0x204+1
#define MotoYaw_ID 0x206   //0x204+2

//热量控制相关
#define BodanMotor_Up_ID 0x203    
#define SHOOTING_HEAT_ID 0x706   //接收枪口热量数据的报头ID

//辅瞄相关
#define ARMOR_NO_AIM	0xff	//没有找到目标
#define ARMOR_AIMED		0x30	//有目标

typedef struct GYRO
{
	float AX;
	float AY;
	float AZ;
	float GX;
	float GY;
	float GZ;
	float PITCH;
	float ROLL;
	float YAW_ABS;
	float Temperature;
	volatile int8_t MPUrecvd;
	volatile int8_t MPUDisconnectCnt;
	ZeroCheck_Typedef zerocheck_yaw;
	ZeroCheck_Typedef zerocheck_pitch;

	float YAW_INC;
	float PITCH_INC;

} gyro_Typedef;


typedef struct
{
    float RCPitch;
    float RCYaw;
    float RCdistance;
    short ReceiveFromTx2BulletCnt;
    short FrictionWheel_speed;
    short DisConnect;
}PC_Receive_t;



void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);
void PCReceive(uint8_t Buf[]);

#endif //__TASK_DATARECEIVE_H
