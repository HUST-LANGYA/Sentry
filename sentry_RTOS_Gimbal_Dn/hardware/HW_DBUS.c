#include "main.h"
#include "HW_DBUS.h"

RC_Ctl_t RC_Ctl;
extern volatile uint8_t sbus_rx_buffer[2][RX_USART3_BUFFER];

/**
  * @brief  遥控器消息解码
  * @param  None
  * @retval None 	
  */ 
void RemoteReceive(volatile uint8_t * const ptr_sbus_rx_buffer)
{
    if(ptr_sbus_rx_buffer ==NULL)
        return ;

	RC_Ctl.rc.ch0 = (ptr_sbus_rx_buffer[0]| (ptr_sbus_rx_buffer[1] << 8)) & 0x07ff; //!< Channel 0
	RC_Ctl.rc.ch1 = ((ptr_sbus_rx_buffer[1] >> 3) | (ptr_sbus_rx_buffer[2] << 5)) & 0x07ff; //!< Channel 1
	RC_Ctl.rc.ch2 = ((ptr_sbus_rx_buffer[2] >> 6) | (ptr_sbus_rx_buffer[3] << 2) | (ptr_sbus_rx_buffer[4] << 10)) & 0x07ff;//!< Channel 2
	RC_Ctl.rc.ch3 = ((ptr_sbus_rx_buffer[4] >> 1) | (ptr_sbus_rx_buffer[5] << 7)) & 0x07ff; //!< Channel 3
	RC_Ctl.rc.s1 = ((ptr_sbus_rx_buffer[5] >> 6)& 0x0003); //!< Switch left
	RC_Ctl.rc.s2 = ((ptr_sbus_rx_buffer[5] >> 4)& 0x0003); //!< Switch right

//	RC_Ctl.mouse.x = ptr_sbus_rx_buffer[6] | (ptr_sbus_rx_buffer[7] << 8); //!< Mouse X axis
//	RC_Ctl.mouse.y = ptr_sbus_rx_buffer[8] | (ptr_sbus_rx_buffer[9] << 8); //!< Mouse Y axis
//	RC_Ctl.mouse.z = ptr_sbus_rx_buffer[10] | (ptr_sbus_rx_buffer[11] << 8); //!< Mouse Z axis
//	RC_Ctl.mouse.press_l = ptr_sbus_rx_buffer[12]; //!< Mouse Left Is Press ?
//	RC_Ctl.mouse.press_r = ptr_sbus_rx_buffer[13]; //!< Mouse Right Is Press ?

//	RC_Ctl.key.w_ = ptr_sbus_rx_buffer[14]&0x01; // KeyBoard value
//	RC_Ctl.key.s_ = (ptr_sbus_rx_buffer[14]>>1)&0x01;
//	RC_Ctl.key.a_ = (ptr_sbus_rx_buffer[14]>>2)&0x01;
//	RC_Ctl.key.d_ = (ptr_sbus_rx_buffer[14]>>3)&0x01;
//	RC_Ctl.key.shift_ =(ptr_sbus_rx_buffer[14]>>4)&0x01;
//	RC_Ctl.key.ctrl_ = (ptr_sbus_rx_buffer[14]>>5)&0x01;
//	RC_Ctl.key.q_ = (ptr_sbus_rx_buffer[14]>>6)&0x01;
//	RC_Ctl.key.e_ = (ptr_sbus_rx_buffer[14]>>7)&0x01;	
//	RC_Ctl.key.r_ = (ptr_sbus_rx_buffer[15])&0x01;
//	RC_Ctl.key.f_ = (ptr_sbus_rx_buffer[15]>>1)&0x01;
//	RC_Ctl.key.g_ = (ptr_sbus_rx_buffer[15]>>2)&0x01; 
//	RC_Ctl.key.z_ = (ptr_sbus_rx_buffer[15]>>3)&0x01;
//	RC_Ctl.key.x_ = (ptr_sbus_rx_buffer[15]>>4)&0x01;
//	RC_Ctl.key.c_ = (ptr_sbus_rx_buffer[15]>>5)&0x01;
//	RC_Ctl.key.v_ = (ptr_sbus_rx_buffer[15]>>6)&0x01;
//	RC_Ctl.key.b_ = (ptr_sbus_rx_buffer[15]>>7)&0x01;
	
	RC_Ctl.RCrecvd = 1; //数据接收标志位
	RC_Ctl.RCDisconnectCnt = 0;    //断线数据清零
	
//	if((RC_Ctl.rc.ch0-1024<15)&&(RC_Ctl.rc.ch0-1024>-15)) RC_Ctl.rc.ch0=1024;
//	if((RC_Ctl.rc.ch1-1024<15)&&(RC_Ctl.rc.ch1-1024>-15)) RC_Ctl.rc.ch1=1024;
//	if((RC_Ctl.rc.ch2-1024<10)&&(RC_Ctl.rc.ch2-1024>-10)) RC_Ctl.rc.ch2=1024;
//	if((RC_Ctl.rc.ch3-1024<10)&&(RC_Ctl.rc.ch3-1024>-10)) RC_Ctl.rc.ch3=1024;     
}

/**
  * @brief  初始化从遥控器传来的参数
  * @param  None
  * @retval None
  */
void reset_remote_control_msg(void)
{
	RC_Ctl.rc.ch0 = 1024;
	RC_Ctl.rc.ch1 = 1024;
	RC_Ctl.rc.ch2 = 1024;
	RC_Ctl.rc.ch3 = 1024;
    RC_Ctl.rc.s1 = 2;
	RC_Ctl.rc.s2 = 2;
//	RC_Ctl.mouse.x = 0;
//	RC_Ctl.mouse.y = 0;
//	RC_Ctl.mouse.z = 0;
//	RC_Ctl.mouse.press_l = 0;
//	RC_Ctl.mouse.press_r = 0;
	
//	RC_Ctl.key.w_ = 0;
//	RC_Ctl.key.s_ = 0;                            
//	RC_Ctl.key.a_ = 0;
//	RC_Ctl.key.d_ = 0;
//	RC_Ctl.key.q_ = 0;
//	RC_Ctl.key.e_ = 0;
//	RC_Ctl.key.r_ = 0;
//	RC_Ctl.key.f_ = 0;
//	RC_Ctl.key.shift_ = 0;
//	RC_Ctl.key.ctrl_ = 0;
	
}

/**
  * @brief  获取遥控器值
  * @param  None
  * @retval 遥控器结构体
  */
inline RC_Ctl_t getRCData(void)
{
	return RC_Ctl;
}
