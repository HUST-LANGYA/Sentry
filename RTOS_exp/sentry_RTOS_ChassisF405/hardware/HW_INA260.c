#include "main.h"
#include "HW_INA260.h"
INA260 INA260_1,INA260_2;

//������
/*inline*/ void INA_READ_Current(void)
{
	float Cur;
	Cur = INA260_Read(INA260_1_ID<<1,REG_CURRENT);
	INA260_1.Current = (float)LSB_VOLTAGE*Cur;
    Cur = INA260_Read(INA260_2_ID<<1,REG_CURRENT);
	INA260_2.Current = (float)LSB_VOLTAGE*Cur;
}

//����ѹ
/*inline*/ void INA_READ_Vol(void)
{
	float Vol;
	Vol = INA260_Read(INA260_1_ID<<1,REG_VOLTAGE);
	INA260_1.Voltage = (float)LSB_VOLTAGE*Vol;
    Vol = INA260_Read(INA260_2_ID<<1,REG_VOLTAGE);
	INA260_2.Voltage = (float)LSB_VOLTAGE*Vol;
}
//������
/*inline*/ void INA_READ_Power(void)
{
	uint16_t power;
	power = INA260_Read(INA260_1_ID<<1,REG_POWER);
	INA260_1.Power = (float)LSB_POWER *power;
    power = INA260_Read(INA260_2_ID<<1,REG_POWER);
	INA260_2.Power = (float)LSB_POWER *power;
}

//����⼸��inline���벻ͨ���Ļ����ͻ��ɴ��κ꺯����


//������Ĵ���
int16_t INA260_Read(uint8_t address, uint8_t reg)
{ 
    uint8_t templ=0;
	uint8_t temph=0;
	int16_t temp;
	uint8_t ack ;
    IIC_Start();
    IIC_Send_Byte(address); //���͵͵�ַ
    ack = IIC_Wait_Ack();
    IIC_Send_Byte(reg); //���͵͵�ַ
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(address + 1); //�������ģʽ 
    IIC_Wait_Ack();
    delay_us(25);  //���Ӵ˴���ͨ�ųɹ�������
    temph=IIC_Read_Byte();  //���Ĵ��� 3
	IIC_Ack();
	templ=IIC_Read_Byte();  //���Ĵ��� 3
	IIC_NAck();
    IIC_Stop();//����һ��ֹͣ���� 
	temp = (int16_t)temph<<8|templ;
    return temp;
}

//д����Ĵ���
void INA260_WriteOneByte(uint8_t address,uint8_t reg,uint8_t command)
{ 
    IIC_Start();
    IIC_Send_Byte(address);  //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);//���͸ߵ�ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(command); //���͵͵�ַ
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
}


