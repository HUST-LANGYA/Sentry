#ifndef __TASK_ACTION_UPDATE_H
#define __TASK_ACTION_UPDATE_H

typedef struct
{
	int16_t ChassisMode;
	int16_t ShootMode;
	int16_t GimbalMode;    
}Status_t;
typedef struct
{
	uint32_t BullectCnt;
	
	uint32_t Pitch_Last_Cnt;
	uint32_t Yaw_Last_Cnt;
	uint32_t Chassis_Last_Cnt;
	uint32_t Bodan_Last_Cnt;
	uint32_t Gyro_Last_Cnt;
	uint32_t PC_Last_Cnt;
	uint32_t Judge_Last_Cnt;
	uint32_t FrictionLeft_Last_Cnt;
	uint32_t FrictionRight_Last_Cnt;
	uint32_t Bullect_Last_Cnt;
	uint32_t Drone_Last_Cnt;
	
	uint32_t Pitch_Disconnect_Cnt;
	uint32_t Yaw_Disconnect_Cnt;
	uint32_t Chassis_Disconnect_Cnt;
	uint32_t Bodan_Disconnect_Cnt;
	uint32_t Gyro_Disconnect_Cnt;
	uint32_t PC_Disconnect_Cnt;
	uint32_t Judge_Disconnect_Cnt;
	uint32_t FrictionLeft_Disconnect_Cnt;
	uint32_t FrictionRight_Disconnect_Cnt;
	uint32_t NoBullect_Cnt;
	uint32_t Drone_Disconnect_Cnt;

	
	uint32_t Pitch_block;
	uint32_t Yaw_block;
	uint32_t Bodan_block;
	uint32_t Friction_block;
	
	uint8_t is_pitch_down;
	uint8_t is_yaw_down;
	uint8_t is_chassis_down;
	uint8_t is_bodan_down;
	uint8_t is_gyro_down;
	uint8_t is_pc_down;
	uint8_t is_judge_down;
	uint8_t is_friction_down;
	uint8_t is_drone_down;

	uint8_t is_pitch_block;
	uint8_t is_yaw_block;
	uint8_t is_bodan_block;
	uint8_t is_friction_block;
	
	uint8_t is_left_limit,is_right_limit;

	uint8_t is_no_bullet;
}block_disconnect_t;




#define Down 0x0000

//底盘模式
#define Chassis_RC 0x01
#define Chassis_PC 0x02

//射击模式
#define Shoot_Dn_RC 0x04
#define Shoot_Dn_PC 0x08
#define Shoot_Dn_SLEEP 0x00

//云台模式
#define Gimbal_Dn_RC 0x04
#define Gimbal_Dn_PC 0x08
#define Gimbal_Dn_SLEEP 0x00

void Status_Act(void);
void action_update(void);
void Change_Check(void);
void DisconnectDetect(void);
void remote_reset(void);


#endif //__TASK_ACTION_UPDATE_H
