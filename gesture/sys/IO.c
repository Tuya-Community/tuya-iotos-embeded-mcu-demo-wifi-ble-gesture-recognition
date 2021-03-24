#include "IO.h"
#include "delay.h"
#include "TIM.h"
#include "math.h"
#include "stdlib.h"
// 2灯 1通风 1加热  1浇水 1加水 1加湿 2个IIC 温湿度 光照 2个ADC  水位 土壤湿度
#if 1//IIC
void IIC_Init(void)
{
	IIC_SCL_OUT;
	IIC_SDA_OUT;
}
void IIC_Start(void)//产生IIC起始信号
{
	IIC_SDA_OUT;     //sda线输出
	IIC_SDA_SET;//IIC_SDA=1;	  	  
	IIC_SCL_SET;//IIC_SCL=1;
	delay_us(5);
	IIC_SDA_RESET;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	IIC_SCL_RESET;//IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
void IIC_Stop(void)//产生IIC停止信号
{
	IIC_SDA_OUT;//sda线输出
	IIC_SCL_RESET;//IIC_SCL=0;
	IIC_SDA_RESET;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(5);
	IIC_SCL_SET;//IIC_SCL=1; 
	delay_us(5);
	IIC_SDA_SET;//IIC_SDA=1;//发送I2C总线结束信号				   	
}
void IIC_Ack(void)//产生ACK应答
{
	IIC_SCL_RESET;//IIC_SCL=0;
	IIC_SDA_OUT;//SDA_OUT();
	IIC_SDA_RESET;//IIC_SDA=0;
	delay_us(5);
	IIC_SCL_SET;//IIC_SCL=1;
	delay_us(5);
	IIC_SCL_RESET;//IIC_SCL=0;
}   
void IIC_NAck(void)//不产生ACK应答	
{
	IIC_SCL_RESET;//IIC_SCL=0;
	IIC_SDA_OUT;//SDA_OUT();
	IIC_SDA_SET;//IIC_SDA=1;
	delay_us(5);
	IIC_SCL_SET;//IIC_SCL=1;
	delay_us(5);
	IIC_SCL_RESET;//IIC_SCL=0;
}	
uint8_t IIC_Wait_Ack(void)//等待应答信号到来:1,接收应答失败;0,接收应答成功
{
	uint8_t ucErrTime=0;
	IIC_SDA_IN;      //SDA设置为输入     
	IIC_SCL_SET;//IIC_SCL=1;
	delay_us(5);	 
	while(IIC_SDA_State)//检测SDA是否仍为高电平
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_RESET;//IIC_SCL=0;
	return 0;  
} 
void IIC_Send_Byte(uint8_t txd)//IIC发送一个字节; 先发送高位
{                        
	uint8_t t;   
	IIC_SDA_OUT; 	    
	IIC_SCL_RESET;//IIC_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
			//IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_SET;//IIC_SDA=1;
		else
			IIC_SDA_RESET;//IIC_SDA=0;
		
		txd<<=1; 	  
		delay_us(5);   //对TEA5767这三个延时都是必须的
		IIC_SCL_SET;//IIC_SCL=1;
		delay_us(5); 
		IIC_SCL_RESET;//IIC_SCL=0;	
		delay_us(5);
	}	 
} 
uint8_t IIC_Read_Byte(unsigned char ack)//读一个字节，可加是否应答位,1加ack，0不加ack 从高位开始读
{
	unsigned char i,receive=0;
	IIC_SDA_IN;//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		IIC_SCL_RESET;// IIC_SCL=0; 
		delay_us(5);
		IIC_SCL_SET;//IIC_SCL=1;
		receive<<=1;
		if(IIC_SDA_State)
			receive++;   
		delay_us(5); 
	}					 
		if (ack)
			IIC_Ack(); //发送ACK
		else
			IIC_NAck();//发送nACK   
		return receive;
}

uint8_t IIC_Write_Byte(uint8_t DrvAddr,uint16_t WriteAddr,uint8_t data)//直接写一个字节
{
	uint8_t ret=0;
	IIC_Start();

	IIC_Send_Byte(DrvAddr);	    //发送写命令
	ret |= IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);//发送地址	  
	ret |= IIC_Wait_Ack();		
	IIC_Send_Byte(data);     //发送字节							   
	ret |= IIC_Wait_Ack(); 

	IIC_Stop();
	delay_us(10);
	return ret;
}
uint8_t IIC_ReadMulByte(uint8_t DrvAddr,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)//读字节
{  	    																 
	uint8_t ret=0;
	
	IIC_Start();  
	IIC_Send_Byte(DrvAddr);	   //发送写命令
	ret |= IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);//发送地址	    
	ret |= IIC_Wait_Ack();	    
	
	IIC_Start();
	IIC_Send_Byte(DrvAddr+1);           //进入接收模式			   
	ret |= IIC_Wait_Ack();
	while(NumToRead)
	{
		if(NumToRead==1)
		{
			*pBuffer=IIC_Read_Byte(0);	
		}
		else
		{
			*pBuffer=IIC_Read_Byte(1);
		}
		pBuffer++;
		NumToRead--;
	}
	IIC_Stop();//产生一个停止条件	
	return ret;	
}
uint8_t IIC_WriteMulByte(uint8_t DrvAddr,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)//可一次写多个字节
{
	uint8_t ret=0;
	IIC_Start();

	IIC_Send_Byte(DrvAddr);	    //发送写命令
	ret |= IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);//发送地址	  
	ret |= IIC_Wait_Ack();		

	while(NumToWrite--)
	{ 										  		   
		IIC_Send_Byte(*pBuffer);     //发送字节							   
		ret |= IIC_Wait_Ack(); 
		pBuffer++;
	}
	IIC_Stop();
	delay_us(10);
	return ret;
}
#endif
/******************************************************手势识别******************************************************/
uint8_t F_TASK_PAJ7620U2=0;
#if USING_PAJ7620U2
const unsigned char PAJ7620Global_InitArray[][2] ={
  {0xEF,0x00},
	{0x37,0x07},
  {0x38,0x17},
	{0x39,0x06},
	{0x41,0x00},
	{0x42,0x00},
	{0x46,0x2D},
	{0x47,0x0F},
	{0x48,0x3C},
	{0x49,0x00},
	{0x4A,0x1E},
	{0x4C,0x20},
	{0x51,0x10},
	{0x5E,0x10},
	{0x60,0x27},
	{0x80,0x42},
	{0x81,0x44},
	{0x82,0x04},
	{0x8B,0x01},
	{0x90,0x06},
	{0x95,0x0A},
	{0x96,0x0C},
	{0x97,0x05},
	{0x9A,0x14},
	{0x9C,0x3F},
	{0xA5,0x19},
	{0xCC,0x19},
	{0xCD,0x0B},
	{0xCE,0x13},
	{0xCF,0x64},
	{0xD0,0x21},
	{0xEF,0x01},
	{0x02,0x0F},
	{0x03,0x10},
	{0x04,0x02},
	{0x25,0x01},
	{0x27,0x39},
	{0x28,0x7F},
	{0x29,0x08},
	{0x3E,0xFF},
	{0x5E,0x3D},
	{0x65,0x96},
	{0x67,0x97},
	{0x69,0xCD},
	{0x6A,0x01},
	{0x6D,0x2C},
	{0x6E,0x01},
	{0x72,0x01},
	{0x73,0x35},
	{0x74,0x00},
	{0x77,0x01},
};


const unsigned char PAJ7620Gesture_InitArray[][2] ={
	{0xEF,0x00},
	{0x41,0x00},
	{0x42,0x00},
	{0xEF,0x00},
	{0x48,0x3C},
	{0x49,0x00},
	{0x51,0x10},
	{0x83,0x20},
	{0x9F,0xF9},
	{0xEF,0x01},
	{0x01,0x1E},
	{0x02,0x0F},
	{0x03,0x10},
	{0x04,0x02},
	{0x41,0x40},
	{0x43,0x30},
	{0x65,0x96},
	{0x66,0x00},
	{0x67,0x97},
	{0x68,0x01},
	{0x69,0xCD},
	{0x6A,0x01},
	{0x6B,0xB0},
	{0x6C,0x04},
	{0x6D,0x2C},
	{0x6E,0x01},
	{0x74,0x00},
	{0xEF,0x00},
	{0x41,0xFF},
	{0x42,0x01},
};
void PAJ7620_SelectBank(uint8_t Bank)
{
	IIC_Write_Byte(PAJ7620_ID,0xef,Bank);
}

void PAJ7620_WakeUp(void)
{
	uint8_t WakeFlag=0x00;
	while(WakeFlag!=0x20)
	{
		uint8_t i=0;
		IIC_Start();
		IIC_Send_Byte(PAJ7620_ID);
		IIC_Stop();
		delay_ms(5);
		IIC_Start();
		IIC_Send_Byte(PAJ7620_ID);
		IIC_Stop();
		PAJ7620_SelectBank(Bank0);
		IIC_ReadMulByte(PAJ7620_ID,0x00,&WakeFlag,1);
		i++;
		if(i>2)
		{
			break;
		}
	}
}
void PAJ7620_Init( )
{
	uint8_t Init_ArraySize;
	PAJ7620_WakeUp();
	PAJ7620_SelectBank(Bank0);
	for(Init_ArraySize=0;Init_ArraySize<51;Init_ArraySize++)
		IIC_Write_Byte(PAJ7620_ID,PAJ7620Global_InitArray[Init_ArraySize][0],PAJ7620Global_InitArray[Init_ArraySize][1]);
	PAJ7620_SelectBank(Bank0);	
	
	PAJ7620_WakeUp();
	PAJ7620_SelectBank(Bank0);
	for(Init_ArraySize=0;Init_ArraySize<30;Init_ArraySize++)
		IIC_Write_Byte(PAJ7620_ID,PAJ7620Gesture_InitArray[Init_ArraySize][0],PAJ7620Gesture_InitArray[Init_ArraySize][1]);
	PAJ7620_SelectBank(Bank0);
}
uint8_t PAJ7620_Gesture1_Detected(void)
{
	uint8_t Gesture_Data;
	IIC_ReadMulByte(PAJ7620_ID,0x43,&Gesture_Data,1);
	PAJ7620_SelectBank(Bank0);
	return Gesture_Data;
}


unsigned char S_UP[]={"UP"};
unsigned char S_DOWN[]={"DOWN"};
unsigned char S_LEFT[]={"LEFT"};
unsigned char S_RIGHT[]={"RIGHT"};
unsigned char S_FORWARD[]={"FORWARD"};
unsigned char S_BACKWARD[]={"BACKWARD"};
void TASK_PAJ7620U2(void)
{
	uint8_t state=0;
	state=PAJ7620_Gesture1_Detected();
	if(state&GES_UP)
		mcu_dp_string_update(DPID_GESTURE,S_UP,strlen((void*)S_UP));
	 if(state&GES_DOWN)
		mcu_dp_string_update(DPID_GESTURE,S_DOWN,strlen((void*)S_DOWN));
	 if(state&GES_LEFT)
		mcu_dp_string_update(DPID_GESTURE,S_LEFT,strlen((void*)S_LEFT));
	 if(state&GES_RIGHT)
		mcu_dp_string_update(DPID_GESTURE,S_RIGHT,strlen((void*)S_RIGHT));
	 if(state&GES_FORWARD)
		mcu_dp_string_update(DPID_GESTURE,S_FORWARD,strlen((void*)S_FORWARD));
	 if(state&GES_BACKWARD)
		mcu_dp_string_update(DPID_GESTURE,S_BACKWARD,strlen((void*)S_BACKWARD));
	
}
#endif
/******************************************************手势识别******************************************************/
void Modules_Init(void)
{
	IIC_Init();
	PAJ7620_Init();
}
void SwitchIO_Init(void)
{
	LED_4_OUT;	
}
void IO_Init(void)
{
	Modules_Init();
	SwitchIO_Init();
}
