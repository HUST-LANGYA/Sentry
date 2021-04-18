#ifndef __TASK_CHASSIS_H
#define __TASK_CHASSIS_H

#define ChassisCurrentLimit 16000
#define ChassisMaxSpeed 5800
#define ChassisTempSpeed 150


#define L_direction 1
#define R_direction 0

#define SENTRY_SCANNING 0   //哨兵巡逻模式

//底盘运动方向
#define FORWARD  0x00
#define BACKWARD 0x01

typedef struct
{
	uint8_t MovingDir;
	int32_t Journey;
	uint8_t is_shootable;
	uint8_t is_heat_ok;    
	uint8_t PC_DownFlag;	//关机标志
	uint8_t State;
	float BulletSpeedSet;
	uint8_t position;   	//0 1 2
	uint8_t HurtDir;
	uint8_t aim_pos;
	uint8_t hero_alarm;
	int32_t FrictionRealSpeed[2];
	int16_t KS_103_DIS;
}Robot;//这个结构体感觉跟裁判系统的返回信息有关系


typedef struct
{
	uint16_t angle_abs;//0~8191(0x1fff)
	int16_t real_speed;//实际转速
    int16_t real_flow; //实际电流
	PID_Typedef pid;
	ZeroCheck_Typedef zerocheck;
	int32_t angle_inc;
}_3508_motor_t;//3508参数结构体



//void PID_Chassis_Init(void);
//void Chassis_RC_Act(void);
//void Chassis_Patrol_Act(void);
//void Chassis_PID_Cal (void);
//void action_update(void);
//void remote_reset(void);
//
////void ChassisZeroCheck(void);
//void Disconnect_Check(void);
//_3508_motor_t* Get_MotorChassis(void);
//block_disconnect_t* Get_block_t(void);
//int16_t* GetChassisSetspeed(void);

void task_Chassis(void* parameter);
//void PID_Chassis_ClearError(void);  

#endif //__TASK_CHASSIS_H
