#include "main.h"
#include "HW_LimitSwitch.h"

//��δȷ���Ƿ���Ҫ�г̿���

/**
 * @brief  �����г̿��ص�������������
 * @param  None
 * @retval None
 */
void GPIO_Config_Limit_Switch(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpioInit.GPIO_Mode = /*GPIO_Mode_IPU;*/GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_14;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpioInit);
}


///**
// * @brief  ���������г̿��صļ��ֵ
// * @param  None
// * @retval None
// */
///*inline*/ void limitSwitch_detect(void)
//{//PB3��PB13���г̿��ص�����
//    extern block_disconnect_t block_disconnect;
//    block_disconnect.is_left_limit = (LimitSw_L)?1:0;
//    block_disconnect.is_right_limit = (LimitSw_R)?1:0;
//}