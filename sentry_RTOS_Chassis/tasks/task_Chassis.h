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
void PID_Chassis_ClearError(void);

static void Chassis_Patrol_Act(void);
static void Chassis_RC_Act(void);
static void Chassis_SLEEP_Act(void);
static void Chassis_DEBUG_Act(void);
static void Chassis_Patrol_PID_Cal(void);
static void Chassis_RC_PID_Cal(void);
inline static void Chassis_SLEEP_PID_Cal(void);
static void Chassis_DEBUG_PID_Cal(void);
static void PID_Chassis_Init(void);

#endif //__TASK_CHASSIS_H
