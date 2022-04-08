/*********************************************************************************
* 文件内容: 定义所有通信检测所需要的函数
* 作    者: Hellocoded
* 时    间: 2021.11.20
*********************************************************************************/


#include "Com_Check.h"


//外部变量
extern uint8_t Offet_Check_Flag;
extern uint8_t is_game_start;
extern uint8_t aim_flag;
//内部变量
uint8_t Offet_ComSend[NUMBER_SEND];
uint8_t Send_Init_Flag=0;
//uint8_t Offet_Check_Flag;
block_disconnet_t block_disconnet;
uint8_t Check_ID[MAX_NUM] = {CAN_MOTOR_PITCH,CAN_MOTOR_YAW,CAN_GYRO_PITCH,CAN_GYRO_YAW,CAN_CHASSIS,CAN_MOTOR_BODAN,CAN_MOTOR_FRIC,UART_PC,UART_REMOTE,UART_JUDGE};




/******************************************************
* 函数功能: 检测初始化
* 传 入 值: 无
* 传 出 值: 无
******************************************************/
void Offet_Reset(void)
{
   block_disconnet.PC_Rec_Last_Cnt = 0; //PC
	 block_disconnet.PC_Send_Last_Cnt = 0;
	 block_disconnet.PC_Rec_Disconnect_Cnt = 0;
	 block_disconnet.PC_Send_Disconnect_Cnt = 0;
	 block_disconnet.is_pc_rec_down = 0;
	 block_disconnet.is_pc_send_down = 0;
	 
	 block_disconnet.Judge_Rec_Disconnect_Cnt = 0; //裁判系统
	 block_disconnet.Judge_Send_Disconnect_Cnt = 0;
	 block_disconnet.Judge_Send_Last_Cnt = 0;
	 block_disconnet.Judge_Rec_Last_Cnt = 0;
	 block_disconnet.is_judge_send_down = 0;
	 block_disconnet.is_judge_rec_down = 0;
	
	 block_disconnet.Yaw_Rec_Last_Cnt = 0; //YAW
	 block_disconnet.Yaw_Send_Last_Cnt = 0;
	 block_disconnet.Yaw_Rec_Disconnect_Cnt = 0;
	 block_disconnet.Yaw_Send_Disconnect_cnt = 0;
	 block_disconnet.is_yaw_rec_down = 0;
	 block_disconnet.is_yaw_send_down = 0;
	
	 block_disconnet.Pitch_Rec_Last_Cnt = 0;
	 block_disconnet.Pitch_Send_Last_Cnt = 0;
	 block_disconnet.Pitch_Rec_Disconnect_Cnt = 0;
	 block_disconnet.Pitch_Send_Disconnect_cnt= 0;
	 block_disconnet.is_pitch_rec_down = 0;
	 block_disconnet.is_pitch_send_down = 0;
	
	 block_disconnet.Bodan_Rec_Last_Cnt = 0;
	 block_disconnet.Bodan_Send_Last_Cnt = 0;
	 block_disconnet.Bodan_Rec_Disconnect_Cnt = 0;
	 block_disconnet.Bodan_Send_Disconnect_Cnt = 0;
	 block_disconnet.is_bodan_rec_down = 0;
	 block_disconnet.is_bodan_send_down = 0;
	 
	
	 block_disconnet.Gyro_Rec_Last_Cnt = 0;
	 block_disconnet.Gyro_Send_Last_Cnt = 0;
	 block_disconnet.Gyro_Send_Disconnect_Cnt = 0;
	 block_disconnet.Gyro_Rec_Disconnect_Cnt = 0;
	 block_disconnet.is_gyro_rec_down  = 0;
	 block_disconnet.is_gyro_send_down = 0;
	 
	 block_disconnet.Chassis_Rec_Last_Cnt = 0;
	 block_disconnet.Chassis_Send_Last_Cnt = 0;
	 block_disconnet.Chassis_Rec_Disconnect_Cnt = 0;
	 block_disconnet.Drone_Disconnect_Send_Cnt = 0;
	 block_disconnet.is_chassis_rec_down = 0;
	 block_disconnet.is_chassis_send_down = 0;
	 
	 block_disconnet.Bullect_Rec_Last_Cnt = 0;
	 block_disconnet.BullectCnt = 0;
	 block_disconnet.Bodan_Rec_Disconnect_Cnt = 0;
	 block_disconnet.is_no_bullet = 0;
	
   block_disconnet.FrictionLeft_Rec_Last_Cnt = 0;
	 block_disconnet.FrictionLeft_Send_Last_Cnt = 0;
	 block_disconnet.FrictionLeft_Rec_Disconnect_Cnt = 0;
	 block_disconnet.FrictionLeft_Send_Disconnect_Cnt = 0;
	 block_disconnet.FrictionRight_Rec_Last_Cnt = 0;
	 block_disconnet.FrictionRight_Send_Last_Cnt = 0;
	 block_disconnet.FrictionRight_Rec_Disconnect_Cnt = 0;
	 block_disconnet.FrictionRight_Send_Disconnect_Cnt = 0;
	 block_disconnet.is_friction_rec_down = 0;
	 block_disconnet.is_friction_send_down = 0;



}




/*******************************************************
* 函数功能:通信模块全部检测掉线检测
* 传 入 值: 无
* 输 出 值: 无
*******************************************************/
uint32_t timenow;
void Offet_Judge(void)
{
	   
	 timenow = xTaskGetTickCount();
	
	if(timenow - block_disconnet.PC_Rec_Last_Cnt>100)		//pc
		block_disconnet.PC_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.PC_Rec_Disconnect_Cnt = 0;
	if(timenow - block_disconnet.PC_Send_Last_Cnt > 100)
		block_disconnet.PC_Send_Disconnect_Cnt++;
	else 
		block_disconnet.PC_Send_Disconnect_Cnt = 0;
	block_disconnet.is_pc_rec_down = (block_disconnet.PC_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
	block_disconnet.is_pc_send_down = (block_disconnet.PC_Send_Disconnect_Cnt > 5)? ('Y'): ('N');
//	
	
	if(timenow - block_disconnet.Judge_Rec_Last_Cnt>100)	//裁判系统
		block_disconnet.Judge_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Judge_Rec_Disconnect_Cnt = 0;
	if(timenow - block_disconnet.Judge_Send_Last_Cnt > 100)
		block_disconnet.Judge_Send_Disconnect_Cnt++;
	else 
		block_disconnet.Judge_Send_Disconnect_Cnt = 0;
	block_disconnet.is_judge_rec_down = (block_disconnet.Judge_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
	block_disconnet.is_judge_send_down = (block_disconnet.Judge_Send_Disconnect_Cnt > 5)? ('Y') : ('N');
	
	
	if(timenow - block_disconnet.Yaw_Rec_Last_Cnt>100)		//yaw离线检测
		block_disconnet.Yaw_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Yaw_Rec_Disconnect_Cnt = 0;
	if(timenow - block_disconnet.Yaw_Send_Last_Cnt > 100)
	  block_disconnet.Yaw_Send_Disconnect_cnt++;
	else 
    block_disconnet.Yaw_Send_Disconnect_cnt = 0;
  block_disconnet.is_yaw_rec_down = (block_disconnet.Yaw_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
	block_disconnet.is_yaw_send_down = (block_disconnet.Yaw_Send_Disconnect_cnt > 5)? ('Y'): ('N'); 
	
	                      
	if(timenow - block_disconnet.Pitch_Rec_Last_Cnt>100)		//pitch离线检测
		block_disconnet.Pitch_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Pitch_Rec_Disconnect_Cnt = 0;				
  if(timenow - block_disconnet.Pitch_Send_Last_Cnt > 100)
    block_disconnet.Pitch_Send_Disconnect_cnt++;
  else
    block_disconnet.Pitch_Send_Disconnect_cnt = 0;		
	block_disconnet.is_pitch_rec_down = (block_disconnet.Pitch_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
  block_disconnet.is_pitch_send_down = (block_disconnet.Pitch_Send_Disconnect_cnt > 5)? ('Y') : ('N');	
	
	
	if(timenow - block_disconnet.Bodan_Rec_Last_Cnt>100)		//拨弹电机离线检测
		block_disconnet.Bodan_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Bodan_Rec_Disconnect_Cnt = 0;					
	if(timenow - block_disconnet.Bodan_Send_Last_Cnt > 100)
	  block_disconnet.Bodan_Send_Disconnect_Cnt++;
  else
    block_disconnet.Bodan_Send_Disconnect_Cnt = 0;		
	block_disconnet.is_bodan_rec_down = (block_disconnet.Bodan_Rec_Disconnect_Cnt > 5)? ('Y') :('N');	
	block_disconnet.is_bodan_send_down = (block_disconnet.Bodan_Send_Disconnect_Cnt > 5)? ('Y') : ('N');
	
	
	if(timenow - block_disconnet.Gyro_Rec_Last_Cnt>100)		//云台
		block_disconnet.Gyro_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Gyro_Rec_Disconnect_Cnt = 0;					
	if(timenow - block_disconnet.Gyro_Send_Last_Cnt >100)
		block_disconnet.Gyro_Send_Disconnect_Cnt++;
	else 
		block_disconnet.Gyro_Send_Disconnect_Cnt = 0;
  block_disconnet.is_gyro_rec_down = (block_disconnet.Gyro_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
	block_disconnet.is_gyro_send_down = (block_disconnet.Gyro_Send_Disconnect_Cnt > 5)? ('Y') : ('N');
	
	if(timenow - block_disconnet.Chassis_Rec_Last_Cnt > 100) //底盘通信
		block_disconnet.Chassis_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.Chassis_Rec_Disconnect_Cnt = 0;
	if(timenow - block_disconnet.Chassis_Send_Last_Cnt>100)
	  block_disconnet.Chassis_Send_Disconnect_Cnt++;
	else
		block_disconnet.Chassis_Send_Disconnect_Cnt = 0;
	block_disconnet.is_chassis_rec_down = (block_disconnet.Chassis_Rec_Disconnect_Cnt > 5)? ('Y'):('N');
	block_disconnet.is_chassis_send_down = (block_disconnet.Chassis_Send_Disconnect_Cnt > 5)? ('Y') : ('N');
	
	
	if(block_disconnet.BullectCnt - block_disconnet.Bullect_Rec_Last_Cnt>100)		//No bullect
		block_disconnet.NoBullect_Cnt++;
	else 
		block_disconnet.NoBullect_Cnt = 0;					
	if(block_disconnet.NoBullect_Cnt>=10)
		block_disconnet.is_no_bullet = 'Y';
	else 
		block_disconnet.is_no_bullet = 'N';
	
	
	if(timenow - block_disconnet.FrictionLeft_Rec_Last_Cnt>100)		//fritction离线检测
		block_disconnet.FrictionLeft_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.FrictionLeft_Rec_Disconnect_Cnt = 0;	
  if(timenow - block_disconnet.FrictionLeft_Send_Last_Cnt > 100)
	  block_disconnet.FrictionLeft_Send_Disconnect_Cnt++;
	else
		block_disconnet.FrictionLeft_Send_Disconnect_Cnt = 0;
	
	if(timenow - block_disconnet.FrictionRight_Rec_Last_Cnt>100)		
		block_disconnet.FrictionRight_Rec_Disconnect_Cnt++;
	else 
		block_disconnet.FrictionRight_Rec_Disconnect_Cnt = 0;	
	if(timenow - block_disconnet.FrictionRight_Send_Disconnect_Cnt >100)
		block_disconnet.FrictionRight_Send_Disconnect_Cnt++;
	else
		block_disconnet.FrictionRight_Send_Disconnect_Cnt++;
	if(block_disconnet.FrictionLeft_Rec_Disconnect_Cnt>=5 || block_disconnet.FrictionRight_Rec_Disconnect_Cnt>=5)
		block_disconnet.is_friction_rec_down = 'Y';
	else 
		block_disconnet.is_friction_rec_down = 'N';
	if(block_disconnet.FrictionLeft_Send_Disconnect_Cnt >= 5 || block_disconnet.FrictionRight_Rec_Disconnect_Cnt >= 5)
    block_disconnet.is_friction_send_down = 'Y';
	else 
		block_disconnet.is_friction_send_down = 'N';
	
   if(timenow - block_disconnet.Remote_Rec_Last_Cnt > 100)
		 block_disconnet.Remote_Rec_Disconnect_Cnt++;
	 else
		 block_disconnet.Remote_Rec_Disconnect_Cnt = 0;
	 block_disconnet.is_remote_down = (block_disconnet.Remote_Rec_Disconnect_Cnt > 5)? ('Y') : ('N');
	 
	 
	 /*
	  需要补充的检测模块
    
	 
	 */
}


/*******************************************************
* 函数功能: 检测通信数组发送至手机APP
* 传 入 值: 无 
* 输 出 值: 无
*******************************************************/

void Offet_Send(void)
{
	  if(Send_Init_Flag == 0)
	  {
		   for(int i;i<NUMBER_SEND;i++)
			  Offet_ComSend[i] = '0'; //取到0的时候为无效的数据
		 
		}
	
	  if(block_disconnet.Send_Cmd == 'A') //对应第一个界面
		{
			  Offet_ComSend[0] = 'A';
			 Offet_ComSend[1] =  block_disconnet.is_pc_rec_down;
			 Offet_ComSend[2] =  block_disconnet.is_pc_send_down;
			 Offet_ComSend[3] =  block_disconnet.is_judge_rec_down;
			 Offet_ComSend[4] =  block_disconnet.is_judge_send_down;
			 Offet_ComSend[5] =  block_disconnet.is_yaw_rec_down;
			 Offet_ComSend[6] =  block_disconnet.is_yaw_send_down;
			 Offet_ComSend[7] =  block_disconnet.is_pitch_rec_down;
			 Offet_ComSend[8] =  block_disconnet.is_pitch_send_down;
			 Offet_ComSend[9] =  block_disconnet.is_bodan_rec_down;
			 Offet_ComSend[10] = block_disconnet.is_bodan_send_down;
			 Offet_ComSend[11] = block_disconnet.is_gyro_rec_down;
			 Offet_ComSend[12] = block_disconnet.is_gyro_send_down;
			 Offet_ComSend[13] = block_disconnet.is_chassis_rec_down;
			 Offet_ComSend[14] = block_disconnet.is_chassis_send_down;
			 Offet_ComSend[15] = block_disconnet.is_friction_rec_down;
			 Offet_ComSend[16] = block_disconnet.is_friction_send_down;
			 Offet_ComSend[17] = block_disconnet.is_no_bullet;
			 Offet_ComSend[18] = block_disconnet.is_remote_down;
			
		   USART_SendArray(UART4,Offet_ComSend,NUMBER_SEND);
			 block_disconnet.Send_Cmd = 0;
		}
		else if(block_disconnet.Send_Cmd == 'B')//自定义通信检测对象和相关的顺序，对应第二个界面
		{
		   
		   //请自行添加要检测的对象，格式如上，注意赋值的顺序要和检测APP上填写的一致,不然会对应不上
		   USART_SendArray(UART4,Offet_ComSend,NUMBER_SEND);
			 block_disconnet.Send_Cmd = 0;
		
		}
}


