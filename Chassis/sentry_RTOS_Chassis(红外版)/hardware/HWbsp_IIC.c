#include "main.h"
#include "stm32f10x.h"
void i2c_init(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	gpioInit.GPIO_Pin=GPIO_Pin_10;
	gpioInit.GPIO_Mode=GPIO_Mode_Out_OD;
	gpioInit.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gpioInit);
	GPIO_SetBits(GPIOC,GPIO_Pin_10);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	gpioInit.GPIO_Pin=GPIO_Pin_15;
	gpioInit.GPIO_Mode=GPIO_Mode_Out_OD;
	gpioInit.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpioInit);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ�� 1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0; 
	IIC_SDA=1;delay_us(1);
	SDA_IN();      //SDA����Ϊ����	   
	IIC_SCL=1;delay_us(1); 	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			//IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
}

void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 

uint8_t IIC_Read_Byte(void)
{
	uint8_t i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(2); 
    }
	return receive;
}

void SDA_OUT(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	gpioInit.GPIO_Pin=GPIO_Pin_15;
	gpioInit.GPIO_Mode=GPIO_Mode_Out_PP;
	gpioInit.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpioInit);
}


void SDA_IN(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	gpioInit.GPIO_Pin=GPIO_Pin_15;
	gpioInit.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpioInit);
}
