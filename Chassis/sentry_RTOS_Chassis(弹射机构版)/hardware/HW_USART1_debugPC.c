#include "main.h"
#include "HW_USART1_debugPC.h"

// 尝试用这个串口给电脑输出数据
// USART1用来定时像电脑发送CPU使用率的信息
// 这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

// 因为USART2好像没引出来，所以暂时还是用USART1试一波裁判系统
uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize]; //从裁判系统接收的数据

uint16_t last_remainHP, cur_remainHP;
uint16_t is_Under_Attack = 0; //如果受击，则将该变量置于某个单稳态的值

//uint8_t JudgeSendBuffer[JudgeSendBufSize];          //向裁判系统发出的数据

/**
  * @brief  USART1配置
  * @param  None
  * @retval None
  */
void USART1_Configuration(void)
{
	USART_InitTypeDef uartInit;
	GPIO_InitTypeDef gpioInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	gpioInit.GPIO_Pin = GPIO_Pin_6;
	//gpioInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_7;
	//gpioInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);

	//  GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET);
	//    GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_RESET);
	//        GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
	//    GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET);

	uartInit.USART_BaudRate = 115200;
	uartInit.USART_WordLength = USART_WordLength_8b;
	uartInit.USART_StopBits = USART_StopBits_1;
	uartInit.USART_Parity = USART_Parity_No;
	uartInit.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &uartInit);

	USART_ClearFlag(USART1, USART_FLAG_TXE | USART_FLAG_TC);

	//USART_ITConfig(USART1,USART_IT_TC,ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	NVIC_InitTypeDef nvicInit;
	nvicInit.NVIC_IRQChannel = USART1_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInit);

	{
		DMA_InitTypeDef dmaInit;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Channel5);

		nvicInit.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);

		dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);
		dmaInit.DMA_MemoryBaseAddr = (uint32_t)JudgeReceiveBuffer;
		dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
		dmaInit.DMA_BufferSize = JudgeBufBiggestSize;
		dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaInit.DMA_Mode = DMA_Mode_Circular; //原先是DMA_Mode_Circular;，但我的理解用Normal模式更合理吧？
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;

		DMA_Init(DMA1_Channel5, &dmaInit);
		DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE); //个人感觉，这个中断开归开，但不要在这个里面处理接收的数据会比较好
		DMA_Cmd(DMA1_Channel5, ENABLE);

		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // 开启串口DMA接收  接受可以提前开启
		USART_Cmd(USART1, ENABLE);
	}
}

////重定向c库函数printf到串口，重定向后可使用printf函数
//int fputc(int ch, FILE *f)
//{
//    USART_SendData(USART1, (uint8_t)ch);  // 发送一个字节数据到串口
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// 等待发送完毕
//    return (ch);
//}

////重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
//int fgetc(FILE *f)
//{
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// 等待串口输入数据
//    return (int)USART_ReceiveData(USART1);
//}

/**
  * @brief  USART1中断服务函数(接收裁判系统的各种数据)
  * @param  None
  * @retval None
  */
uint8_t Judge_Decode_Flag = 0;
void USART1_IRQHandler(void)
{
	//    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	//    {
	//        USART_ClearITPendingBit(USART1,USART_IT_TC);
	//    }

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{

		(void)USART1->SR;
		(void)USART1->DR; //清空数据寄存器（读一遍之后，自动清除IDLE）
						  //        DMA_Cmd(DMA1_Channel5, DISABLE); //先关DMA
						  ////        DMA_ClearFlag( DMA1_FLAG_TE5 );  // 清DMA标志位

		//            JudgeBuffReceive(JudgeReceiveBuffer,JudgeBufBiggestSize);

		////
		////
		//        DMA_SetCurrDataCounter(DMA1_Channel5, JudgeBufBiggestSize);  //重新装填待传输的字节数（不过我感觉 循环模式下，不手动装填应该也行吧？）
		//        DMA_Cmd(DMA1_Channel5, ENABLE);  //重开DMA
	}
}

/**********************************************************************************************************
*函 数 名: JudgeBuffReceive
*功能说明: 裁判系统接收函数
*形    参: ReceiveBuffer[]  DataLen
*返 回 值: 无
**********************************************************************************************************/
//无人机交互数据相关
//TODO: 把这里的int换成int8_t int16_t 之类的
int usr_data_cmd_id;
int sender_ID;
int receiver_ID;

uint8_t *usr_data;
uint8_t test_usr_data;

extern uint8_t cur_escape_period;
extern uint16_t is_Under_Attack;

uint8_t SaveBuffer[2 * JudgeBufBiggestSize];
tGameInfo JudgeReceive;
uint16_t Judgement_DisConnect;
uint16_t cmd_id;
short PackPoint;

uint16_t num_judge_pack = 0, num_001_pack = 0, num_201_pack = 0, num_202_pack = 0;
uint8_t is_game_start;

void JudgeBuffReceive(unsigned char ReceiveBuffer[], uint16_t DataLen)
{

	memcpy(&SaveBuffer[JudgeBufBiggestSize], &ReceiveBuffer[0], JudgeBufBiggestSize); //把ReceiveBuffer[0]地址拷贝到SaveBuffer[24], 依次拷贝24个, 把这一次接收的存到数组后方
	for (PackPoint = 0; PackPoint < JudgeBufBiggestSize; PackPoint++)				  //先处理前半段数据(在上一周期已接收完成)
	{
		if (SaveBuffer[PackPoint] == 0xA5)
		{
			if ((Verify_CRC8_Check_Sum(&SaveBuffer[PackPoint], 5) == 1)) //frame_header 五位数据校验
			{
				cmd_id = (SaveBuffer[PackPoint + 6]) & 0xff;
				cmd_id = (cmd_id << 8) | SaveBuffer[PackPoint + 5];
				DataLen = SaveBuffer[PackPoint + 2] & 0xff;
				DataLen = (DataLen << 8) | SaveBuffer[PackPoint + 1];
				//机器人状态数据
				num_judge_pack++;
				if ((cmd_id == 0x0001) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.game_type = SaveBuffer[PackPoint + 7 + 0] & 0x0F;			  //取出低四位
					JudgeReceive.game_progress = (SaveBuffer[PackPoint + 7 + 0] & 0xF0) >> 4; //取出高四位
					//JudgeReceive.remain_time = SaveBuffer[PackPoint + 7 + 1];
                    is_game_start = (JudgeReceive.game_type == 0x01 && JudgeReceive.game_progress >=0x04)?1:0;
                    
                    num_001_pack++;
				}
				if ((cmd_id == 0x0201) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.robot_id = SaveBuffer[PackPoint + 7 + 0]; //  3 4 5 红方机器人  13 14 15 蓝方机器人
					JudgeReceive.RobotLevel = SaveBuffer[PackPoint + 7 + 1];
					last_remainHP = cur_remainHP;
					JudgeReceive.remainHP = (SaveBuffer[PackPoint + 7 + 3] << 8) | SaveBuffer[PackPoint + 7 + 2];
					cur_remainHP = JudgeReceive.remainHP;
					JudgeReceive.maxHP = (SaveBuffer[PackPoint + 7 + 5] << 8) | SaveBuffer[PackPoint + 7 + 4];
					JudgeReceive.HeatCool17_0 = (SaveBuffer[PackPoint + 7 + 7] << 8) | SaveBuffer[PackPoint + 7 + 6];
					JudgeReceive.HeatMax17_0 = (SaveBuffer[PackPoint + 7 + 9] << 8) | SaveBuffer[PackPoint + 7 + 8];
					JudgeReceive.BulletSpeedMax17_0 = (SaveBuffer[PackPoint + 7 + 11] << 8) | SaveBuffer[PackPoint + 7 + 10]; //高低位突然相反
					JudgeReceive.HeatCool17_2 = (SaveBuffer[PackPoint + 7 + 12] << 8) | SaveBuffer[PackPoint + 7 + 13];
					JudgeReceive.HeatMax17_2 = (SaveBuffer[PackPoint + 7 + 14] << 8) | SaveBuffer[PackPoint + 7 + 15];
					JudgeReceive.BulletSpeedMax17_2 = (SaveBuffer[PackPoint + 7 + 16] << 8) | SaveBuffer[PackPoint + 7 + 17];
					JudgeReceive.MaxPower = (SaveBuffer[PackPoint + 7 + 25] << 8) | SaveBuffer[PackPoint + 7 + 24]; //高低位突然相反
					num_201_pack++;
				}
				//实时功率热量数据
				if ((cmd_id == 0x0202) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					memcpy(&JudgeReceive.realChassisOutV, &SaveBuffer[PackPoint + 7 + 0], 2);
					JudgeReceive.realChassisOutV = JudgeReceive.realChassisOutV / 1000.0f;
					memcpy(&JudgeReceive.realChassisOutA, &SaveBuffer[PackPoint + 7 + 2], 2);
					memcpy(&JudgeReceive.realChassisPower, &SaveBuffer[PackPoint + 7 + 4], 4);
					memcpy(&JudgeReceive.remainEnergy, &SaveBuffer[PackPoint + 7 + 8], 2);
					memcpy(&JudgeReceive.shooterHeat17_0, &SaveBuffer[PackPoint + 7 + 10], 2);
					memcpy(&JudgeReceive.shooterHeat17_2, &SaveBuffer[PackPoint + 7 + 12], 2);
					num_202_pack++;
				}
				//                //机器人增益
				//				if ((cmd_id == 0x0204) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				//				{
				//					JudgeReceive.RobotBuffState = SaveBuffer[PackPoint + 7];
				//					JudgeReceive.HPBuff = JudgeReceive.RobotBuffState & 0x01;
				//					JudgeReceive.CoolBuff = (JudgeReceive.RobotBuffState & 0x02) >> 1;
				//					JudgeReceive.DefenceBuff = (JudgeReceive.RobotBuffState & 0x04) >> 2;
				//					JudgeReceive.AttackBuff = (JudgeReceive.RobotBuffState & 0x08) >> 3;
				//				}
				//受击类型数据
				if ((cmd_id == 0x0206) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.hurt_armor_id = SaveBuffer[PackPoint + 7 + 0] & 0x0F;
					JudgeReceive.hurt_type = (SaveBuffer[PackPoint + 7 + 0] & 0xF0) >> 4;
				}
				//实时射击信息
				if ((cmd_id == 0x0207) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					if ((SaveBuffer[PackPoint + 7 + 0] == 1) && (SaveBuffer[PackPoint + 7 + 1] == 1))
					{
						JudgeReceive.bulletFreq = SaveBuffer[PackPoint + 7 + 2];
						memcpy(&JudgeReceive.bulletSpeed, &SaveBuffer[PackPoint + 7 + 3], 4);
					}
				}
				if ((cmd_id == 0x0301) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{ //飞机给一个信号，让哨兵伪受击，开始底盘运动
					sender_ID = SaveBuffer[PackPoint + 7 + 3] << 4 | SaveBuffer[PackPoint + 7 + 2];
					receiver_ID = SaveBuffer[PackPoint + 7 + 5] << 4 | SaveBuffer[PackPoint + 7 + 4];
					usr_data_cmd_id = SaveBuffer[PackPoint + 7 + 1] << 4 | SaveBuffer[PackPoint + 7 + 0];
					usr_data = SaveBuffer + PackPoint + 7 + 6;
					if (sender_ID == 6 && receiver_ID == 7)
					{
						test_usr_data = (usr_data[0] & 0xFF);
						if ((usr_data[0] & 0xFF) == 0xFF)
						{
							is_Under_Attack = 10000;
							cur_escape_period = 0;
						}
					}
					else if (sender_ID == 106 && receiver_ID == 107)
					{
						is_Under_Attack = 10000;
						cur_escape_period = 0;
					}
				}
			}
		}
		Judgement_DisConnect = 0;
		//有点变向双缓冲的味道
		memcpy(&SaveBuffer[0], &SaveBuffer[JudgeBufBiggestSize], JudgeBufBiggestSize); //把SaveBuffer[24]地址拷贝到SaveBuffer[0], 依次拷贝24个，把之前存到后面的数据提到前面，准备处理
		
	}
}

/**
  * @brief  DMA接收中断
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC5) != RESET)
	{
		JudgeBuffReceive(JudgeReceiveBuffer, JudgeBufBiggestSize);
		DMA_ClearFlag(DMA1_FLAG_TC5);
	}
}

///**
//  * @brief  发送一个字节
//  * @param  USART外设地址
//  *         待发送的字符
//  * @retval none
//  */
//void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
//{
//    USART_SendData(pUSARTx, ch);  //发送一个字节数据到USART->TDR
//    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//等待发送数据寄存器TDR清空
//}

///**
//  * @brief  发送8位的数组
//  * @param  USART外设地址
//  *         待发送的数组头地址
//  *         数组中元素的个数
//  * @retval none
//  */
//void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
//{
////    DMA_Cmd(DMA1_Stream4,DISABLE);
////    DMA_SetCurrDataCounter(DMA1_Stream4, num);
////    DMA_Cmd(DMA1_Stream4,ENABLE);
//    for (uint16_t i = 0; i < num; i++)
//    {
//        USART_SendByte(pUSARTx, *(array+i)); //放一个byte到USART->TDR
//    }
//
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// 等待发送完成
////    DMA_Cmd(DMA1_Stream4,DISABLE);
//}

///**
//  * @brief  发送字符串
//  * @param  USART外设地址
//  *         字符串头地址
//  * @retval none
//  */
//void USART_SendString(USART_TypeDef *pUSARTx, char *str)
//{
//    for(uint16_t k = 0;*(str+k) != '\0';k++)
//        USART_SendByte(pUSARTx, *(str + k));

//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;  // 等待发送完成
//}

///**
//  * @brief  发送一个16位数
//  * @param  USART外设地址
//  *         16位数
//  * @retval none
//  */
//void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    temp_h = (ch & 0XFF00) >> 8; /* 取出高八位 */
//    temp_l = ch & 0XFF;   /* 取出低八位 */

//    USART_SendByte(pUSARTx, temp_h); /* 发送高八位 */
//    USART_SendByte(pUSARTx, temp_l);  /* 发送低八位 */
//}
