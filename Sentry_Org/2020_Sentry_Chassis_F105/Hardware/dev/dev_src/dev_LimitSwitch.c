#include "main.h"

/**
 * @brief  更新左右行程开关的检测值
 * @param  None
 * @retval None
 */
void limitSwitch_detect(void)
{//PB3和PB13接行程开关的输入
    block_disconnect.is_left_limit = (KEY_LEFT)?1:0;
    block_disconnect.is_right_limit = (KEY_RIGHT)?1:0;
}



