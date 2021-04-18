#include "main.h"

INA260 INA260_1/*,INA260_2*/;
void INA_READ_Current(void)
{
	float Cur;
	Cur = INA260_Read(0x40<<1,REG_CURRENT);
	INA260_1.Current = LSB_VOLTAGE*Cur;
}

void INA_READ_Vol(void)
{
	float Vol;
	Vol = INA260_Read(0x40<<1,REG_VOLTAGE);
	INA260_1.Voltage = LSB_VOLTAGE*Vol;
}

void INA_READ_Power(void)
{
	uint16_t power;
	power = INA260_Read(0x40<<1,REG_POWER);
	INA260_1.Power = LSB_POWER*power;
}



int16_t INA260_Read(uint8_t address, uint8_t reg)
{ 
    uint8_t templ=0;
	uint8_t temph=0;
	int16_t temp;
	uint8_t ack ;
    IIC_Start();
    IIC_Send_Byte(address); //发送低地址
    ack = IIC_Wait_Ack();
    IIC_Send_Byte(reg); //发送低地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(address + 1); //进入接收模式 
    IIC_Wait_Ack();
    delay_us(25);  //增加此代码通信成功！！！
    temph=IIC_Read_Byte();  //读寄存器 3
	IIC_Ack();
	templ=IIC_Read_Byte();  //读寄存器 3
	IIC_NAck();
    IIC_Stop();//产生一个停止条件 
	temp = (int16_t)temph<<8|templ;
    return temp;
}

void INA260_WriteOneByte(uint8_t address,uint8_t reg,uint8_t command)
{ 
    IIC_Start();
    IIC_Send_Byte(address);  //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);//发送高地址
    IIC_Wait_Ack();
    IIC_Send_Byte(command); //发送低地址
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
}


