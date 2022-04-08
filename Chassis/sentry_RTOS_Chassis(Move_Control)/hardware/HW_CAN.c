#include "main.h"
#include "string.h"

CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**********************CAN1**********************************/
/**
  * @brief  配置CAN1
  * @param  None
  * @retval None
  */
void CAN1_Configuration(void)
{
    GPIO_InitTypeDef gpioInit;
    CAN_InitTypeDef canInit;
    CAN_FilterInitTypeDef canFilterInit;
    NVIC_InitTypeDef nvicInit;

    /* 打开GPIO时钟、AFIO时钟，CAN时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    //	GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);  // CAN1 remap

    /* CAN1 RX PA11 */
    gpioInit.GPIO_Pin = GPIO_Pin_11;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    /* CAN1 TX PA12 */
    gpioInit.GPIO_Pin = GPIO_Pin_12;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    /* CAN1 Enabling interrupt */
    nvicInit.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    nvicInit.NVIC_IRQChannel = CAN1_RX1_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    nvicInit.NVIC_IRQChannel = CAN1_TX_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    /* CAN  BaudRate = RCC_APB1PeriphClock/(CAN_SJW+CAN_BS1+CAN_BS2)/CAN_Prescaler */
    CAN_DeInit(CAN1);
    CAN_StructInit(&canInit);

    canInit.CAN_TTCM = DISABLE;
    canInit.CAN_ABOM = ENABLE;
    canInit.CAN_AWUM = ENABLE;
    canInit.CAN_NART = ENABLE;
    canInit.CAN_RFLM = DISABLE;
    canInit.CAN_TXFP = ENABLE;
    canInit.CAN_Mode = CAN_Mode_Normal;
    //canInit.CAN_Mode=CAN_Mode_LoopBack;
    canInit.CAN_SJW = CAN_SJW_1tq;
    canInit.CAN_BS1 = CAN_BS1_5tq;
    canInit.CAN_BS2 = CAN_BS2_3tq;
    canInit.CAN_Prescaler = 4;

    CAN_Init(CAN1, &canInit); // CAN1
    canFilterInit.CAN_FilterNumber = 0;
    canFilterInit.CAN_FilterMode = CAN_FilterMode_IdList;  // 标识符列表模式
    canFilterInit.CAN_FilterScale = CAN_FilterScale_16bit; // 32位过滤器
    canFilterInit.CAN_FilterIdHigh = 0x201 << 5;           // 过滤器标识符
    canFilterInit.CAN_FilterIdLow = 0x202 << 5;
    canFilterInit.CAN_FilterMaskIdHigh = 0x203 << 5; // 过滤器屏蔽标识符
    canFilterInit.CAN_FilterMaskIdLow = 0x708 << 5;
    canFilterInit.CAN_FilterFIFOAssignment = CAN_FIFO0; // FIFO0指向过滤器
    canFilterInit.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&canFilterInit);


    //这个FIFO1目前还没用到
    canFilterInit.CAN_FilterNumber = 1;
    canFilterInit.CAN_FilterMode = CAN_FilterMode_IdList;  // 标识符屏蔽位模式
    canFilterInit.CAN_FilterScale = CAN_FilterScale_16bit; // 32位过滤器
    canFilterInit.CAN_FilterIdHigh = 0x201 << 5;           // 过滤器标识符
    canFilterInit.CAN_FilterIdLow = 0x202 << 5;
    canFilterInit.CAN_FilterMaskIdHigh = 0x203 << 5; // 过滤器屏蔽标识符
    canFilterInit.CAN_FilterMaskIdLow = 0x204 << 5;
    canFilterInit.CAN_FilterFIFOAssignment = CAN_FIFO1; // FIFO1指向过滤器
    canFilterInit.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&canFilterInit);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
    CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

/**
  * @brief  CAN1接收中断0
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &Can1_rx_message_0);
        CAN1_DataReceive_0();
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}

/**
  * @brief  CAN1接收中断1
  * @param  None
  * @retval None
  */
void CAN1_RX1_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP1) != RESET)
    {
        CAN_Receive(CAN1, CAN_FIFO1, &Can1_rx_message_1);
        CAN1_DataReceive_1();
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
    }
}
/**
  * @brief  CAN1发送中断
  * @param  None
  * @retval None
  */
void CAN1_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1, CAN_IT_TME) != RESET)
    {
        CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
    }
}

/******************************CAN2*****************************************/
/**
  * @brief  配置CAN2
  * @param  None
  * @retval None
  */
void CAN2_Configuration(void)
{
    GPIO_InitTypeDef gpioInit;
    CAN_InitTypeDef canInit;
    CAN_FilterInitTypeDef canFilterInit;
    NVIC_InitTypeDef nvicInit;

    /* 打开GPIO时钟、AFIO时钟，CAN时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

    /* CAN2 RX PB12 */
    gpioInit.GPIO_Pin = GPIO_Pin_12;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
    /* CAN2 TX PB13 */
    gpioInit.GPIO_Pin = GPIO_Pin_13;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);

    /* CAN2 Enabling interrupt */
    nvicInit.NVIC_IRQChannel = CAN2_RX0_IRQn; // FIFO_0
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    /* CAN2 Enabling interrupt */
    nvicInit.NVIC_IRQChannel = CAN2_RX1_IRQn; // FIFO_1
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    /* CAN2 Enabling interrupt */
    nvicInit.NVIC_IRQChannel = CAN2_TX_IRQn; // TX
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    /* CAN  BaudRate = RCC_APB1PeriphClock/(CAN_SJW+CAN_BS1+CAN_BS2)/CAN_Prescaler */
    CAN_DeInit(CAN2);
    CAN_StructInit(&canInit);

    canInit.CAN_TTCM = DISABLE;
    canInit.CAN_ABOM = ENABLE;
    canInit.CAN_AWUM = ENABLE;
    canInit.CAN_NART = ENABLE;
    canInit.CAN_RFLM = DISABLE;
    canInit.CAN_TXFP = ENABLE;
    canInit.CAN_Mode = CAN_Mode_Normal;
    //canInit.CAN_Mode=CAN_Mode_LoopBack;
    canInit.CAN_SJW = CAN_SJW_1tq;
    canInit.CAN_BS1 = CAN_BS1_5tq;
    canInit.CAN_BS2 = CAN_BS2_3tq;
    canInit.CAN_Prescaler = 4;

    CAN_Init(CAN2, &canInit); // CAN2

    canFilterInit.CAN_FilterNumber = 15;                     //
    canFilterInit.CAN_FilterMode = CAN_FilterMode_IdList;    // 标识符屏蔽位模式
    canFilterInit.CAN_FilterScale = CAN_FilterScale_16bit;   // 32位过滤器
    canFilterInit.CAN_FilterIdHigh = Remote_Control_ID << 5; // 过滤器标识符
    canFilterInit.CAN_FilterIdLow =  SHOOTING_HEAT_ID << 5;
    canFilterInit.CAN_FilterMaskIdHigh =  0 <<5;		// 过滤器屏蔽标识符
    canFilterInit.CAN_FilterMaskIdLow =   0<<5;
    canFilterInit.CAN_FilterFIFOAssignment = CAN_FIFO0;      // FIFO0指向过滤器
    canFilterInit.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&canFilterInit);


     canFilterInit.CAN_FilterNumber = 16;                   //
     canFilterInit.CAN_FilterMode = CAN_FilterMode_IdList;  // 标识符屏蔽位模式
     canFilterInit.CAN_FilterScale = CAN_FilterScale_16bit; // 32位过滤器
     canFilterInit.CAN_FilterIdHigh = 0 | CAN_ID_STD;       // 过滤器标识符
     canFilterInit.CAN_FilterIdLow = 0 | CAN_ID_STD;
     canFilterInit.CAN_FilterMaskIdHigh = 0 | CAN_ID_STD; // 过滤器屏蔽标识符
     canFilterInit.CAN_FilterMaskIdLow = 0 | CAN_ID_STD;
     canFilterInit.CAN_FilterFIFOAssignment = CAN_FIFO1; // FIFO1指向过滤器
     canFilterInit.CAN_FilterActivation = ENABLE;
     CAN_FilterInit(&canFilterInit);

    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
    CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
}

/**
  * @brief  CAN2接收中断
  * @param  None
  * @retval None
  */
void CAN2_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
    {
        CAN_Receive(CAN2, CAN_FIFO0, &Can2_rx_message_0);
        CAN2_DataReceive_0();
        CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
    }
}

/**
  * @brief  CAN2接收中断
  * @param  None
  * @retval None
  */
void CAN2_RX1_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2, CAN_IT_FMP1) != RESET)
    {
        CAN_Receive(CAN2, CAN_FIFO1, &Can2_rx_message_1);
        CAN2_DataReceive_1();
        CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
    }
}

/**
  * @brief  CAN2发送中断
  * @param  None
  * @retval None
  */
void CAN2_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2, CAN_IT_TME) != RESET)
    {
        CAN_ClearITPendingBit(CAN2, CAN_IT_TME);
    }
}
