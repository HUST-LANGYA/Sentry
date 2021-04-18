#ifndef __TASK_CHASSIS_H
#define __TASK_CHASSIS_H

#define ChassisCurrentLimit 16000
#define ChassisMaxSpeed 5800
#define ChassisTempSpeed 150
#define L_direction 1
#define R_direction 0

typedef struct
{
	uint16_t angle_abs;//0~8191(0x1fff)
	int16_t real_speed;//    
	PID_Typedef pid;
	ZeroCheck_Typedef zerocheck;
	int32_t angle_inc;
}_3508_motor_t;//3508�����ṹ��

typedef struct
{	
	int32_t Chassis_Last_Cnt;
	int32_t Chassis_Sleep_Cnt;//���̵��糬ʱ��ʱ��
	uint8_t is_Chassis_Sleep;  //���̵����־
	uint32_t Bodan_Up_block;//��ǹ�ܿ�����־�����ݱ�־��������ʱ��
    uint32_t Bodan_Dn_block;//��ǹ�ܿ�����־�����ݱ�־��������ʱ��
	uint8_t is_left_limit,is_right_limit; //�г̿��ش�����־
}block_disconnect_t;//�����������ṹ��

void PID_Chassis_Init(void);
void Chassis_RC_Act(void);
void Chassis_PC_Act(void);
void Chassis_PID_Cal (void);
void action_update(void);
void remote_reset(void);

void ChassisZeroCheck(void);
void Disconnect_Check(void);
_3508_motor_t* Get_MotorChassis(void);
block_disconnect_t* Get_block_t(void);
int16_t* GetChassisSetspeed(void);


#endif //__TASK_CHASSIS_H
