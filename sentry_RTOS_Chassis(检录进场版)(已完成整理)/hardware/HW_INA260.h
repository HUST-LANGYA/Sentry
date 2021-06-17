#ifndef __HW_INA260_H
#define __HW_INA260_H

#define INA260_1_ID 	0x40		//1000000
#define INA260_2_ID 	0x44		//1000100
#define REG_SETTING 	0x00
#define REG_CURRENT		0x01
#define REG_VOLTAGE 	0x02
#define REG_POWER		0x03
#define LSB_CURRENT		1.25	//mA
#define LSB_VOLTAGE 	1.25	//mV
#define LSB_POWER		10		//mW

typedef struct INA260
{
	uint8_t Configuration[2];		//INA260≈‰÷√
	float Current;
	float Voltage;
    float Power;
}INA260;

void INA_READ_Current(void);
void INA_READ_Vol(void);
void INA_READ_Power(void);

#endif //__HW_INA260_H


