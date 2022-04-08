#ifndef __BSP_CAN_H
#define __BSP_CAN_H


#define ChassisMotor_ID 0x201  //现在的底盘只有一个3508了
// #define ChassisMotor_ID1 0x201
// #define ChassisMotor_ID2 0x202
// #define BodanMotor_ID    0x203
// #define PitchMotor_ID    0x206
// #define YawMotor_ID      0x205

void CAN1_Configuration(void);
void CAN2_Configuration(void);

#endif 
