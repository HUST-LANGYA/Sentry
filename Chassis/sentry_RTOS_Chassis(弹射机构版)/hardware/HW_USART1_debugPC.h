#ifndef __HW_USART1_debugPC_H
#define __HW_USART1_debugPC_H

//尝试用这个串口给电脑输出数据
//把UART4用来定时向电脑发送CPU使用率的信息
//这里要对 printf 和 scanf 啥的做一波封装 （方便以后用串口做命令行形式的调试）

#define USART1_TX_MAX_BUFFER  0xff  //127

typedef struct
{
//	char HeatUpdate_NoUpdate;
//	char SpeedUpdate_NoUpdate;
	
    //0x001包头相关
    uint8_t game_type;
    uint8_t game_progress;
    uint16_t remain_time;  
    
    
	//0x201包头相关
	//int FiredBulletNum;
	uint8_t robot_id;
	uint8_t RobotLevel;
	//uint8_t bulletFreq;
	//uint8_t cardType;
	//uint8_t CardIdx;
	uint16_t remainHP;
	uint16_t maxHP;
    uint16_t HeatCool17_0;
    uint16_t HeatMax17_0;
    uint16_t BulletSpeedMax17_0;
    uint16_t HeatCool17_2;
	uint16_t HeatMax17_2;
    uint16_t BulletSpeedMax17_2;
    uint16_t MaxPower;
    
    
    //0x202包头相关
    float realChassisOutV;  //单位mV，按uint16_t 的格式发过来，怎么转成float？
    float realChassisOutA;  //有点问题这两个
    float realChassisPower;
    uint16_t remainEnergy;
    uint16_t shooterHeat17_0;
    uint16_t shooterHeat17_2;
//	uint8_t armor_id;
//	uint8_t hurt_type;
	
    
    //0x204包头相关
    uint8_t RobotBuffState;
    uint8_t HPBuff;
    uint8_t CoolBuff;
    uint8_t DefenceBuff;
    uint8_t AttackBuff;
    
    //0x206包头相关 (伤害状态)
    uint8_t hurt_armor_id;//受击的装甲模块ID号
    uint8_t hurt_type;//受击类型：
    
    //0x207包头相关
    uint8_t bulletType;
    uint8_t shooterId;
    uint8_t  bulletFreq;
    float bulletSpeed;
//	short  HeatTop;
//	uint16_t shooterHeat42;
	
//	uint16_t LastShooterHeat17;
//	uint16_t shooterHeat17;
//	float bulletSpeed;
//	float LastbulletSpeed;
//	float realChassisOutV;
//	float realChassisOutA;
//	float realChassisPower;
//	uint16_t remainEnergy;       //剩余能量
//	float  bulletSpeed17;
//	float bulletSpeed42;
//	float LastbulletSpeed17;
	//ext_game_HP OtherRobot;
//	uint16_t bullet_remaining_num;
} tGameInfo;

void USART1_Configuration(void);
void JudgeBuffReceive(unsigned char ReceiveBuffer[], uint16_t DataLen);

void USART_SendArray(USART_TypeDef* pUSARTx, uint8_t* array, uint16_t num); //发送8位的数组  (没开DMA的版本)

#endif //__HW_USART1_debugPC_H
