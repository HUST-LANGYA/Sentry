#include "main.h"
#include "task_JudgeDecode.h"

/**
 *  @brief ��ȡ����ϵͳ����
 *  @param  ��
 *  @retval ��
 */
void task_JudgeDecode(void)
{
    extern uint8_t JudgeReceiveBuffer_1[JudgeBufBiggestSize];
    extern uint8_t JudgeReceiveBuffer_2[JudgeBufBiggestSize];
    extern uint8_t * cur_JudgeReceiveBuffer;
    extern uint8_t Judge_Decode_Flag;
    while (1)
    {
        if (Judge_Decode_Flag)
        {
            Judge_Receive((cur_JudgeReceiveBuffer==JudgeReceiveBuffer_1)?JudgeReceiveBuffer_1:JudgeReceiveBuffer_2); //����Ӳ���ϵͳ���յ�����
            Judge_Decode_Flag = RESET;
        }
        vTaskDelay(1);
    }
}
