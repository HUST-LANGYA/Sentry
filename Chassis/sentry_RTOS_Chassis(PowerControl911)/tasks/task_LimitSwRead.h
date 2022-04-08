#ifndef _TASK_LIMIT_SW_READ_H
#define _TASK_LIMIT_SW_READ_H

#ifndef LIMIT_SW_STATE_T
#define LIMIT_SW_STATE_T
//定义两个行程开关的状态组合
typedef enum
{
    Limit_Both_IDLE = 0,  //中间游走（即双边空闲）
    Limit_Left_Active = 1,  //左侧撞墙
    Limit_Right_Active = 2, //右侧撞墙
    Limit_Both_Active = 3     //双边触发（一般这个状态是出现问题了
} LimitSWState_t;
#endif //LIMIT_SW_STATE_T

void task_LimitSwRead(void);

#endif //_TASK_LIMIT_SW_READ_H
