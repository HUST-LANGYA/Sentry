#include "main.h"
#include "task_CV_DataSend.h"

uint8_t SendToPC_Buff[PC_SENDBUF_SIZE];
extern int16_t PC_Sendflag;
int16_t chassis_speed;
float chassis_speed_trans;
short chassis_speed_trans_100;
static void USART2_SendtoPC(void);

typedef union{
	TickType_t xTickCount;
	unsigned char data[4];
}time_use;
time_use sendTOPC_time;

/**
  * @brief  定期向TX2发送云台姿态的任务
  * @param  None
  * @retval None
  */
uint32_t TX2_high_water;
void task_CV_DataSend(void *pvParameters)
{

    while (1)
    {
        USART2_SendtoPC();  //定时给PC发送云台的信息
			vTaskDelay(1);    //(1)  //为了和视觉那边进行接发匹配
#if INCLUDE_uxTaskGetStackHighWaterMark
        TX2_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}


/**
  * @brief  编码数据报包发送给TX2
  * @param  None
  * @retval None
  */
extern int SendToTx2BullectCnt;
short pitch;
int yaw;
short down_sampling_rate = 2; //1000为1帧			//4
int PC_TX_num;                //和Pc之间通信的计数器，用于降低通讯帧率
static void USART2_SendtoPC(void)
{
    if (PC_TX_num % down_sampling_rate == 0) //每down_sampling_rate发一次数据，并做同步
    {
        PC_TX_num = 0;
        SendToPC_Buff[0] = '!';
    
        SendToPC_Buff[1] = 0x05;
        extern float Pitch_Actual, Yaw_Actual;
			  extern float Bullet_Speed;
        pitch = (short)(/*Gimbal.Pitch.MotorTransAngle*/ Pitch_Actual * 100);
        yaw = (int)(/*Gimbal.Yaw.Gyro*/ Yaw_Actual * 100);
        chassis_speed_trans = (float)((float)(chassis_speed) / (1503.43f)); //RPM转m/s
			  chassis_speed_trans_100 = (short)(chassis_speed_trans*100);  
			
        SendToPC_Buff[2] = (unsigned char)((pitch >> 8) & 0x00FF);
        SendToPC_Buff[3] = (unsigned char)((pitch)&0x00FF);

        SendToPC_Buff[4] = (unsigned char)((yaw >> 24) & 0x000000FF);
        SendToPC_Buff[5] = (unsigned char)((yaw >> 16) & 0x000000FF);
        SendToPC_Buff[6] = (unsigned char)((yaw >> 8) & 0x000000FF);
        SendToPC_Buff[7] = (unsigned char)((yaw >> 0) & 0x000000FF);

			
			  ///SendToPC_Buff[9] = '#';
//			  SendToPC_Buff[2] = (unsigned char)((yaw >> 24) & 0x000000FF);
//			  SendToPC_Buff[3] = (unsigned char) 
//			
			
			  sendTOPC_time.xTickCount = xTaskGetTickCountFromISR();
				SendToPC_Buff[8] = sendTOPC_time.data[3];
				SendToPC_Buff[9] = sendTOPC_time.data[2];
				SendToPC_Buff[10] = sendTOPC_time.data[1];
				SendToPC_Buff[11] = sendTOPC_time.data[0];
		    SendToPC_Buff[12] = (unsigned char)(chassis_speed_trans_100 >> 8 & 0x00FF);
				SendToPC_Buff[13] = (unsigned char)(chassis_speed_trans_100 & 0x00FF);
		   	SendToPC_Buff[14] = (unsigned char)(((short)Bullet_Speed >> 8) & 0x00FF);
		    SendToPC_Buff[15] = (unsigned char)((short)Bullet_Speed & 0x00FF);
				
		   // SendToPC_Buff[12] = 0x30;
        SendToPC_Buff[17] = '#';
        Append_CRC8_Check_Sum(SendToPC_Buff, 18);
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        DMA_Cmd(DMA1_Stream6, ENABLE);   //在程序里面使能DMA的发送
    }
    if ((PC_TX_num % (down_sampling_rate / 2) == 0) && (PC_TX_num % down_sampling_rate != 0))
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    PC_TX_num++;
}
