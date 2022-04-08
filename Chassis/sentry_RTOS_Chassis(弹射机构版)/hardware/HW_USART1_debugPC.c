#include "main.h"
#include "HW_USART1_debugPC.h"

// ������������ڸ������������
// USART1������ʱ����Է���CPUʹ���ʵ���Ϣ
// ����Ҫ�� printf �� scanf ɶ����һ����װ �������Ժ��ô�������������ʽ�ĵ��ԣ�

// ��ΪUSART2����û��������������ʱ������USART1��һ������ϵͳ
uint8_t JudgeReceiveBuffer[JudgeBufBiggestSize]; //�Ӳ���ϵͳ���յ�����

uint16_t last_remainHP, cur_remainHP;
uint16_t is_Under_Attack = 0; //����ܻ����򽫸ñ�������ĳ������̬��ֵ

//uint8_t JudgeSendBuffer[JudgeSendBufSize];          //�����ϵͳ����������

/**
  * @brief  USART1����
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
		dmaInit.DMA_Mode = DMA_Mode_Circular; //ԭ����DMA_Mode_Circular;�����ҵ������Normalģʽ������ɣ�
		dmaInit.DMA_Priority = DMA_Priority_VeryHigh;
		dmaInit.DMA_M2M = DMA_M2M_Disable;

		DMA_Init(DMA1_Channel5, &dmaInit);
		DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE); //���˸о�������жϿ��鿪������Ҫ��������洦����յ����ݻ�ȽϺ�
		DMA_Cmd(DMA1_Channel5, ENABLE);

		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // ��������DMA����  ���ܿ�����ǰ����
		USART_Cmd(USART1, ENABLE);
	}
}

////�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
//int fputc(int ch, FILE *f)
//{
//    USART_SendData(USART1, (uint8_t)ch);  // ����һ���ֽ����ݵ�����
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;// �ȴ��������
//    return (ch);
//}

////�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
//int fgetc(FILE *f)
//{
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;// �ȴ�������������
//    return (int)USART_ReceiveData(USART1);
//}

/**
  * @brief  USART1�жϷ�����(���ղ���ϵͳ�ĸ�������)
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
		(void)USART1->DR; //������ݼĴ�������һ��֮���Զ����IDLE��
						  //        DMA_Cmd(DMA1_Channel5, DISABLE); //�ȹ�DMA
						  ////        DMA_ClearFlag( DMA1_FLAG_TE5 );  // ��DMA��־λ

		//            JudgeBuffReceive(JudgeReceiveBuffer,JudgeBufBiggestSize);

		////
		////
		//        DMA_SetCurrDataCounter(DMA1_Channel5, JudgeBufBiggestSize);  //����װ���������ֽ����������Ҹо� ѭ��ģʽ�£����ֶ�װ��Ӧ��Ҳ�аɣ���
		//        DMA_Cmd(DMA1_Channel5, ENABLE);  //�ؿ�DMA
	}
}

/**********************************************************************************************************
*�� �� ��: JudgeBuffReceive
*����˵��: ����ϵͳ���պ���
*��    ��: ReceiveBuffer[]  DataLen
*�� �� ֵ: ��
**********************************************************************************************************/
//���˻������������
//TODO: �������int����int8_t int16_t ֮���
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

	memcpy(&SaveBuffer[JudgeBufBiggestSize], &ReceiveBuffer[0], JudgeBufBiggestSize); //��ReceiveBuffer[0]��ַ������SaveBuffer[24], ���ο���24��, ����һ�ν��յĴ浽�����
	for (PackPoint = 0; PackPoint < JudgeBufBiggestSize; PackPoint++)				  //�ȴ���ǰ�������(����һ�����ѽ������)
	{
		if (SaveBuffer[PackPoint] == 0xA5)
		{
			if ((Verify_CRC8_Check_Sum(&SaveBuffer[PackPoint], 5) == 1)) //frame_header ��λ����У��
			{
				cmd_id = (SaveBuffer[PackPoint + 6]) & 0xff;
				cmd_id = (cmd_id << 8) | SaveBuffer[PackPoint + 5];
				DataLen = SaveBuffer[PackPoint + 2] & 0xff;
				DataLen = (DataLen << 8) | SaveBuffer[PackPoint + 1];
				//������״̬����
				num_judge_pack++;
				if ((cmd_id == 0x0001) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.game_type = SaveBuffer[PackPoint + 7 + 0] & 0x0F;			  //ȡ������λ
					JudgeReceive.game_progress = (SaveBuffer[PackPoint + 7 + 0] & 0xF0) >> 4; //ȡ������λ
					//JudgeReceive.remain_time = SaveBuffer[PackPoint + 7 + 1];
                    is_game_start = (JudgeReceive.game_type == 0x01 && JudgeReceive.game_progress >=0x04)?1:0;
                    
                    num_001_pack++;
				}
				if ((cmd_id == 0x0201) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.robot_id = SaveBuffer[PackPoint + 7 + 0]; //  3 4 5 �췽������  13 14 15 ����������
					JudgeReceive.RobotLevel = SaveBuffer[PackPoint + 7 + 1];
					last_remainHP = cur_remainHP;
					JudgeReceive.remainHP = (SaveBuffer[PackPoint + 7 + 3] << 8) | SaveBuffer[PackPoint + 7 + 2];
					cur_remainHP = JudgeReceive.remainHP;
					JudgeReceive.maxHP = (SaveBuffer[PackPoint + 7 + 5] << 8) | SaveBuffer[PackPoint + 7 + 4];
					JudgeReceive.HeatCool17_0 = (SaveBuffer[PackPoint + 7 + 7] << 8) | SaveBuffer[PackPoint + 7 + 6];
					JudgeReceive.HeatMax17_0 = (SaveBuffer[PackPoint + 7 + 9] << 8) | SaveBuffer[PackPoint + 7 + 8];
					JudgeReceive.BulletSpeedMax17_0 = (SaveBuffer[PackPoint + 7 + 11] << 8) | SaveBuffer[PackPoint + 7 + 10]; //�ߵ�λͻȻ�෴
					JudgeReceive.HeatCool17_2 = (SaveBuffer[PackPoint + 7 + 12] << 8) | SaveBuffer[PackPoint + 7 + 13];
					JudgeReceive.HeatMax17_2 = (SaveBuffer[PackPoint + 7 + 14] << 8) | SaveBuffer[PackPoint + 7 + 15];
					JudgeReceive.BulletSpeedMax17_2 = (SaveBuffer[PackPoint + 7 + 16] << 8) | SaveBuffer[PackPoint + 7 + 17];
					JudgeReceive.MaxPower = (SaveBuffer[PackPoint + 7 + 25] << 8) | SaveBuffer[PackPoint + 7 + 24]; //�ߵ�λͻȻ�෴
					num_201_pack++;
				}
				//ʵʱ������������
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
				//                //����������
				//				if ((cmd_id == 0x0204) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				//				{
				//					JudgeReceive.RobotBuffState = SaveBuffer[PackPoint + 7];
				//					JudgeReceive.HPBuff = JudgeReceive.RobotBuffState & 0x01;
				//					JudgeReceive.CoolBuff = (JudgeReceive.RobotBuffState & 0x02) >> 1;
				//					JudgeReceive.DefenceBuff = (JudgeReceive.RobotBuffState & 0x04) >> 2;
				//					JudgeReceive.AttackBuff = (JudgeReceive.RobotBuffState & 0x08) >> 3;
				//				}
				//�ܻ���������
				if ((cmd_id == 0x0206) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					JudgeReceive.hurt_armor_id = SaveBuffer[PackPoint + 7 + 0] & 0x0F;
					JudgeReceive.hurt_type = (SaveBuffer[PackPoint + 7 + 0] & 0xF0) >> 4;
				}
				//ʵʱ�����Ϣ
				if ((cmd_id == 0x0207) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{
					if ((SaveBuffer[PackPoint + 7 + 0] == 1) && (SaveBuffer[PackPoint + 7 + 1] == 1))
					{
						JudgeReceive.bulletFreq = SaveBuffer[PackPoint + 7 + 2];
						memcpy(&JudgeReceive.bulletSpeed, &SaveBuffer[PackPoint + 7 + 3], 4);
					}
				}
				if ((cmd_id == 0x0301) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
				{ //�ɻ���һ���źţ����ڱ�α�ܻ�����ʼ�����˶�
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
		//�е����˫�����ζ��
		memcpy(&SaveBuffer[0], &SaveBuffer[JudgeBufBiggestSize], JudgeBufBiggestSize); //��SaveBuffer[24]��ַ������SaveBuffer[0], ���ο���24������֮ǰ�浽����������ᵽǰ�棬׼������
		
	}
}

/**
  * @brief  DMA�����ж�
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
//  * @brief  ����һ���ֽ�
//  * @param  USART�����ַ
//  *         �����͵��ַ�
//  * @retval none
//  */
//void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
//{
//    USART_SendData(pUSARTx, ch);  //����һ���ֽ����ݵ�USART->TDR
//    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) ;//�ȴ��������ݼĴ���TDR���
//}

///**
//  * @brief  ����8λ������
//  * @param  USART�����ַ
//  *         �����͵�����ͷ��ַ
//  *         ������Ԫ�صĸ���
//  * @retval none
//  */
//void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
//{
////    DMA_Cmd(DMA1_Stream4,DISABLE);
////    DMA_SetCurrDataCounter(DMA1_Stream4, num);
////    DMA_Cmd(DMA1_Stream4,ENABLE);
//    for (uint16_t i = 0; i < num; i++)
//    {
//        USART_SendByte(pUSARTx, *(array+i)); //��һ��byte��USART->TDR
//    }
//
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;// �ȴ��������
////    DMA_Cmd(DMA1_Stream4,DISABLE);
//}

///**
//  * @brief  �����ַ���
//  * @param  USART�����ַ
//  *         �ַ���ͷ��ַ
//  * @retval none
//  */
//void USART_SendString(USART_TypeDef *pUSARTx, char *str)
//{
//    for(uint16_t k = 0;*(str+k) != '\0';k++)
//        USART_SendByte(pUSARTx, *(str + k));

//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) ;  // �ȴ��������
//}

///**
//  * @brief  ����һ��16λ��
//  * @param  USART�����ַ
//  *         16λ��
//  * @retval none
//  */
//void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    temp_h = (ch & 0XFF00) >> 8; /* ȡ���߰�λ */
//    temp_l = ch & 0XFF;   /* ȡ���Ͱ�λ */

//    USART_SendByte(pUSARTx, temp_h); /* ���͸߰�λ */
//    USART_SendByte(pUSARTx, temp_l);  /* ���͵Ͱ�λ */
//}
