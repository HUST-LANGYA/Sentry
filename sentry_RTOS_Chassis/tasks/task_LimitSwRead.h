#ifndef _TASK_LIMIT_SW_READ_H
#define _TASK_LIMIT_SW_READ_H

#ifndef LIMIT_SW_STATE_T
#define LIMIT_SW_STATE_T
//���������г̿��ص�״̬���
typedef enum
{
    Limit_Both_IDLE = 0,  //�м����ߣ���˫�߿��У�
    Limit_Left_Active = 1,  //���ײǽ
    Limit_Right_Active = 2, //�Ҳ�ײǽ
    Limit_Both_Active = 3     //˫�ߴ�����һ�����״̬�ǳ���������
} LimitSWState_t;
#endif //LIMIT_SW_STATE_T

void task_LimitSwRead(void);

#endif //_TASK_LIMIT_SW_READ_H
