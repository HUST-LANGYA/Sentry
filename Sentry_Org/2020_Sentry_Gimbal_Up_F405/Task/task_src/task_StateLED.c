#include "main.h"

/**
 *  @brief  根据掉线检测的结果闪状态灯
 *  @param  无
 *  @retval 无
 */
uint32_t blinkTick;//闪灯计数变量 
void blink_state(void)
{
    blinkTick++;
    
    if(blinkTick % 1000 == 0)
        ;
    if(blinkTick % 2000 == 0)
        ;
    if(blinkTick % 3000 == 0)
        ;
    if(blinkTick % 500  == 0)
        ;
    
}

