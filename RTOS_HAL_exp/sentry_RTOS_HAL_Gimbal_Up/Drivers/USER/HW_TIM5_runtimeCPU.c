#include "main.h"
#include "HW_TIM5_runtimeCPU.h"

/* 中断服务函数：用于统计运行时间 */

volatile uint32_t CPU_RunTime = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim5)
        CPU_RunTime++;
}
