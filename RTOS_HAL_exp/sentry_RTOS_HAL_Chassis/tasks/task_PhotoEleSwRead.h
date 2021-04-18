#ifndef __TASK_PHOTO_ELE_SW_READ_H
#define __TASK_PHOTO_ELE_SW_READ_H

//定义两个光电开关的状态组合
typedef enum {
    Both_IDLE   = 0,   //中间游走（即双边空闲）
    Left_Active = 1,   //左侧撞墙
    Right_Active= 2,   //右侧撞墙
    Both_Active = 3    //双边触发（一般这个状态是出现问题了
} PhotoEleState_t;

void task_PhotoEleSwRead(void);

#endif //__TASK_PHOTO_ELE_SW_READ_H
