#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//����̨���ձ��ĵ�ID
#define Gyro_Pitch_ID 0x100   //�����Ƿ���pitch���ĵ�ID
#define Gyro_Yaw_ID   0x101   //�����Ƿ���yaw���ĵ�ID

#define MotoPitch_ID 0x205  //pitch����������ĵ�ID
#define MotoYaw_ID 0x207    //yaw����������ĵ�ID

#define BodanMotor_ID 0x203      //��������������ĵ�ID
#define SHOOTING_HEAT_ID 0x706   //����ǹ���������ݵı�ͷID
#define Remote_Control_ID 0x705  //����ң�������ݵı�ͷID

typedef union
{
	float fdata; //4?
	unsigned long idata;
} FloatlongType;  //��������union�����һ��long��float

//void RCDataToSpeed(void); //��ң�������յ�����ת��Ϊδ��pid������ٶ��趨ֵ
typedef struct
{
    int8_t s1,s2;
    int16_t ch[4];
}REMOTE_t;


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

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);
void PCReceive(uint8_t Buf[]);
//void GyroReceiveFunc(uint8_t gyroBuffer[], int16_t BufferNum);

#pragma anon_unions

/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364)
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01 << 0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01 << 1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01 << 2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01 << 3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01 << 4)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01 << 5)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01 << 6)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01 << 7)

/* ----------------------- Data Struct ------------------------------------- */

#endif //__TASK_DATA_RECEIVE_H
