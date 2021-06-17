#include "main.h"
#include "task_CV_DataSend.h"
/*--------------内部变量-------------------*/
uint8_t SendToPC_Buff[PC_SENDBUF_SIZE];
//F405_typedef F405;
/*--------------外部变量-------------------*/
//extern Gimbal_Typedef Gimbal;
extern short PC_Sendflag;

/**********************************************************************************************************
*函 数 名: task_CV_DataSend
*功能说明: 通信任务
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
uint32_t TX2_high_water;
void task_CV_DataSend(void *pvParameters)
{

    while (1)
    {
        USART2_SendtoPC();

        vTaskDelay(1);

#if INCLUDE_uxTaskGetStackHighWaterMark
        TX2_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}

/**********************************************************************************************************
*函 数 名: Tx2_Off_CheckAndSet
*功能说明: 发送指令给tx2使其关机
*形    参: 指向发送数据的指针
*返 回 值: 无
**********************************************************************************************************/
static void Tx2_Off_CheckAndSet(u8 *Buff)
{
    //	int i;
    //	if(PC_Sendflag == Tx2_Off)
    //	for(i = 0;i < 9;i++)
    //	{
    //		*Buff = '!';
    //		Buff++;
    //	}
}

/**********************************************************************************************************
*函 数 名: USART2_SendtoPC
*功能说明: 发送云台姿态
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
extern int SendToTx2BullectCnt;
short pitch;
int yaw;
short down_sampling_rate = 6; //1000为1帧			//4
int PC_TX_num;                //和Pc之间通信的计数器，用于降低通讯帧率
static void USART2_SendtoPC(void)
{
    if (PC_TX_num % down_sampling_rate == 0) //每down_sampling_rate发一次数据，并做同步
    {
        PC_TX_num = 0;
        SendToPC_Buff[0] = '!';

        SendToPC_Buff[1] = 0x05;
        extern float Pitch_Actual, Yaw_Actual;
        pitch = (short)(/*Gimbal.Pitch.MotorTransAngle*/ Pitch_Actual * 100);
        yaw = (int)(/*Gimbal.Yaw.Gyro*/ Yaw_Actual * 100);

        SendToPC_Buff[2] = (unsigned char)((pitch >> 8) & 0x00FF);
        SendToPC_Buff[3] = (unsigned char)((pitch)&0x00FF);

        SendToPC_Buff[4] = (unsigned char)((yaw >> 24) & 0x000000FF);
        SendToPC_Buff[5] = (unsigned char)((yaw >> 16) & 0x000000FF);
        SendToPC_Buff[6] = (unsigned char)((yaw >> 8) & 0x000000FF);
        SendToPC_Buff[7] = (unsigned char)((yaw >> 0) & 0x000000FF);

        Tx2_Off_CheckAndSet(SendToPC_Buff);

        SendToPC_Buff[9] = '#';
        Append_CRC8_Check_Sum(SendToPC_Buff, 10);

        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        DMA_Cmd(DMA1_Stream6, ENABLE);
    }
    if ((PC_TX_num % (down_sampling_rate / 2) == 0) && (PC_TX_num % down_sampling_rate != 0))
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    PC_TX_num++;
}
