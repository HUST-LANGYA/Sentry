#include "main.h"
#include "task_JudgeDecode.h"

/**
 *  @brief ��ȡ����ϵͳ����
 *  @param  ��
 *  @retval ��
 */
void task_JudgeDecode(void)
{
    extern uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize];
    extern uint8_t Judge_Decode_Flag;
    while (1)
    {
        if (Judge_Decode_Flag)
        {
            Judge_Receive(JudgeReceiveBuffer); //����Ӳ���ϵͳ���յ�����
            Judge_Decode_Flag = RESET;
        }
        vTaskDelay(1);
    }
}
