#ifndef __IO_H
#define __IO_H 		
#include "MY_ST_config.h"
#include "math.h"
#include "stdbool.h"


//IIC_SDA	  PB11
#define IIC_SDA_OUT {RCC->IOPENR|=1<<1;GPIOB->MODER&=~(3<<22);GPIOB->MODER|=1<<22;GPIOB->PUPDR|=1<<22;} 
#define IIC_SDA_IN  {RCC->IOPENR|=1<<1;GPIOB->MODER&=~(3<<22);GPIOB->MODER|=0<<22;} 
#define IIC_SDA_SET GPIOB->ODR|=1<<11
#define IIC_SDA_RESET  GPIOB->ODR&=~(1<<11)
#define IIC_SDA_State ((GPIOB->IDR & 1<<11) == 1<<11)

//IIC_SCL	  PB12
#define IIC_SCL_OUT {RCC->IOPENR|=1<<1;GPIOB->MODER&=~(3<<24);GPIOB->MODER|=1<<24;GPIOB->PUPDR|=1<<24;}  
#define IIC_SCL_IN  {RCC->IOPENR|=1<<1;GPIOB->MODER&=~(3<<24);GPIOB->MODER|=0<<24;} 
#define IIC_SCL_SET GPIOB->ODR|=1<<12
#define IIC_SCL_RESET  GPIOB->ODR&=~(1<<12)
#define IIC_SCL_State ((GPIOB->IDR & 1<<12) == 1<<12)

#if 1
void IIC_Init(void);
void IIC_Start(void);//����IIC��ʼ�ź�
void IIC_Stop(void);//����IICֹͣ�ź�
void IIC_Ack(void);//����ACKӦ��
void IIC_NAck(void);//������ACKӦ��	
uint8_t IIC_Wait_Ack(void);//�ȴ�Ӧ���źŵ���:1,����Ӧ��ʧ��;0,����Ӧ��ɹ�
void IIC_Send_Byte(uint8_t txd);//IIC����һ���ֽ�; �ȷ��͸�λ
uint8_t IIC_Read_Byte(unsigned char ack);//��һ���ֽڣ��ɼ��Ƿ�Ӧ��λ,1��ack��0����ack �Ӹ�λ��ʼ��
uint8_t IIC_Write_Byte(uint8_t DrvAddr,uint16_t WriteAddr,uint8_t data);//ֱ��дһ���ֽ�
uint8_t IIC_ReadMulByte(uint8_t DrvAddr,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);//���ֽ�
uint8_t IIC_WriteMulByte(uint8_t DrvAddr,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);//��һ��д����ֽ�
#endif

//LED_CTRL4	 	PA5
#define LED_4_OUT {RCC->IOPENR|=1<<0;GPIOA->MODER&=~(3<<10);GPIOA->MODER|=1<<10;} 
#define LED_4_SET GPIOA->ODR|=1<<5
#define LED_4_RESET  GPIOA->ODR&=~(1<<5)
#define LED_4_TOG GPIOA->ODR^=1<<5


#if USING_PAJ7620U2/***************************������PAJ7620U2���ƴ�����**************************/



#define PAJ_REGITER_BANK_SEL 0XEF    //BANKѡ��Ĵ���
#define PAJ_BANK0            0X00    //BANK0
#define PAJ_BANK1            0X01    //BANK1

#define PAJ7620_ID 0x73<<1
#define Bank0 0x00
#define Bank1 0x01
#define PAJ7620InitMode_Global 0x01
#define PAJ7620InitMode_Gesture 0x02
#define PAJ7620InitMode_Proximity 0x03



//����ʶ��Ч��
#define BIT(x) 1<<(x)

#define GES_UP              BIT(0) //����
#define GES_DOWN            BIT(1) //����
#define GES_LEFT            BIT(2) //����
#define GES_RIGHT           BIT(3) //����
#define GES_FORWARD         BIT(4) //��ǰ
#define GES_BACKWARD        BIT(5) //���
#define GES_CLOCKWISE       BIT(6) //˳ʱ��
#define GES_COUNT_CLOCKWISE BIT(7) //��ʱ��
#define GES_WAVE            BIT(8) //�Ӷ�




typedef enum{
	BANK0 = 0, //BANK0�Ĵ���
	BANK1,     //BANK1�Ĵ���
}bank_e;

#endif
extern uint8_t F_TASK_PAJ7620U2;
void TASK_PAJ7620U2(void);


void IO_Init(void);
#endif

