#include "main.h"
#include "task_JudgeDecode.h"

/**
 *  @brief 读取裁判系统数据
 *  @param  无
 *  @retval 无
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
            Judge_Receive((cur_JudgeReceiveBuffer==JudgeReceiveBuffer_1)?JudgeReceiveBuffer_1:JudgeReceiveBuffer_2); //处理从裁判系统接收的数据
            Judge_Decode_Flag = RESET;
        }
        vTaskDelay(1);
    }
}
