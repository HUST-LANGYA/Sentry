#ifndef __HW_USART1_debugPC_H
#define __HW_USART1_debugPC_H

//尝试用这个串口给电脑输出数据
//把UART4用来定时向电脑发送CPU使用率的信息
//这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

#define USART1_TX_MAX_BUFFER  0xff  //127

typedef struct
{
	char HeatUpdate_NoUpdate;
	char SpeedUpdate_NoUpdate;
	
	
	int FiredBulletNum;
	uint8_t robot_id;
	uint8_t RobotLevel;
	uint8_t bulletFreq;
	uint8_t cardType;
	uint8_t CardIdx;
	uint16_t remainHP;
	uint16_t maxHP;
	
	
	uint8_t armor_id;
	uint8_t hurt_type;
	
	short  HeatTop;
	uint16_t shooterHeat42;
	
	uint16_t LastShooterHeat17;
	uint16_t shooterHeat17;
	float bulletSpeed;
	float LastbulletSpeed;
	float realChassisOutV;
	float realChassisOutA;
	float realChassispower;
	uint16_t remainEnergy;       //剩余能量
	float  bulletSpeed17;
	float bulletSpeed42;
	float LastbulletSpeed17;
	ext_game_HP OtherRobot;
	uint16_t bullet_remaining_num;
}
tGameInfo;

void USART1_Configuration(void);
void USART_SendArray(USART_TypeDef* pUSARTx, uint8_t* array, uint16_t num); //发送8位的数组  (没开DMA的版本)

#endif //__HW_USART1_debugPC_H
