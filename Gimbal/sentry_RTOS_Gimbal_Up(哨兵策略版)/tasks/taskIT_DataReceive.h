#ifndef __TASK_DATARECEIVE_H
#define __TASK_DATARECEIVE_H

//陀螺仪相关
#define Gyro_Pitch_ID 0x100
#define Gyro_Yaw_ID 0x101

//云台电机相关
#define MotoPitch_ID_Up 0x205
#define MotoYaw_ID_Up 0x206

//摩擦轮电机
#define FrictionMotor_Up_0_ID 0x201
#define FrictionMotor_Up_1_ID 0x202

//热量控制相关
#define BodanMotor_Up_ID 0x203
#define SHOOTING_HEAT_ID 0x706   //接收枪口热量数据的报头ID
//云台协同工作
//#define GIMBAL_UP_ID 0x305
#define GIMBAL_DOWN_ID 0x306

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

typedef struct
{
  float Down_pitch;
	float Down_yaw;
	float Up_Trans_pitch; //pitch角度转化后的输出值
	float Down_distance;
	float Up_Trans_yaw;   //yaw角度转化后的数出值
	uint8_t Valid_flag;
		
}Down_Receive_t;

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);
void PCReceive(uint8_t Buf[]);
void Down_Receive(uint8_t Down_Receiver[]);
#endif //__TASK_DATARECEIVE_H
