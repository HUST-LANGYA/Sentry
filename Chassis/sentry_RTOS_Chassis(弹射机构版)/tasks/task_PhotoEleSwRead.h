#ifndef __TASK_PHOTO_ELE_SW_READ_H
#define __TASK_PHOTO_ELE_SW_READ_H

#ifndef PHOTOELE_SW_STATE_T
#define PHOTOELE_SW_STATE_T
//定义两个光电开关的状态组合
typedef enum
{
    PhotoEle_Both_IDLE = 0,    //中间游走（即双边空闲）
    PhotoEle_Left_Active = 1,  //左侧撞墙
    PhotoEle_Right_Active = 2, //右侧撞墙
    PhotoEle_Both_Active = 3   //双边触发（一般这个状态是出现问题了
} PhotoEleSWState_t;
#endif //PHOTOELE_SW_STATE_T

void task_PhotoEleSwRead(void);

#endif //__TASK_PHOTO_ELE_SW_READ_H
