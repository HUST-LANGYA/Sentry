#include "main.h"

/**
 *  @brief  ���ݵ��߼��Ľ����״̬��
 *  @param  ��
 *  @retval ��
 */
uint32_t blinkTick;//���Ƽ������� 
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

