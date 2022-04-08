#ifndef __TASK_PHOTO_ELE_SW_READ_H
#define __TASK_PHOTO_ELE_SW_READ_H

#ifndef FTIR_STATE_T
#define FTIR_STATE_T
//定义两个光电开关的状态组合

#define K_ACCELERATE 5.0f  //加速的阈值,还需要实测

typedef enum
{
    Ftir_Both_IDLE = 0,    //中间游走（即双边空闲）
    Ftir_Left_Active = 1,  //左侧撞墙
    Ftir_Right_Active = 2, //右侧撞墙
    Ftir_Both_Active = 3   //双边触发（一般这个状态是出现问题了
} FtirState_t;
#endif //PHOTOELE_SW_STATE_T

void task_FtirRead(void);

#endif //__TASK_PHOTO_ELE_SW_READ_H
