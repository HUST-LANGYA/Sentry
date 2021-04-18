#include "main.h"
#include "HW_CAN.h"

CAN_RxHeaderTypeDef CAN1_Rx0_Header,CAN1_Rx1_Header,CAN2_Rx0_Header,CAN2_Rx1_Header;
uint8_t CAN1_Rx0_Msg[8],CAN1_Rx1_Msg[8],CAN2_Rx0_Msg[8],CAN2_Rx1_Msg[8];


/**
  * @brief  CAN接收中断0回调
  * @param  None
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
{
    if(hcan == &hcan1)
    {
        if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_Rx0_Header,CAN1_Rx0_Msg) == HAL_OK)
            CAN1_Rx0_DataDecode();
    }
    else if(hcan == &hcan2)
    {
        if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &CAN2_Rx0_Header,CAN2_Rx0_Msg) == HAL_OK)
            CAN2_Rx0_DataDecode();
    }
}


/**
  * @brief  CAN接收中断1回调
  * @param  None
  * @retval None
  */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan == &hcan1)
    {
        if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &CAN1_Rx1_Header,CAN1_Rx1_Msg) == HAL_OK)
            CAN1_Rx1_DataDecode();
    }
    else if(hcan == &hcan2)
    {
        if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &CAN2_Rx1_Header,CAN2_Rx1_Msg) == HAL_OK)
            CAN2_Rx1_DataDecode();
    }
}
