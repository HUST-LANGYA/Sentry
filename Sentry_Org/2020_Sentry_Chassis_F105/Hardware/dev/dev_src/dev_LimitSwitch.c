#include "main.h"

/**
 * @brief  ���������г̿��صļ��ֵ
 * @param  None
 * @retval None
 */
void limitSwitch_detect(void)
{//PB3��PB13���г̿��ص�����
    block_disconnect.is_left_limit = (KEY_LEFT)?1:0;
    block_disconnect.is_right_limit = (KEY_RIGHT)?1:0;
}



