#include "max6675.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"	

/*max6675初始化程序*/

/*使用两个芯片测量，分开写连个测试函数*/
/*片选不一样，CS1----PB8,cs2---PB9*/
/*数据--输入PB4*/
/*时钟--输出PB3*/
void Max6675Init(void){	
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//DBGMCU->CR = DBGMCU->CR & ~((uint32_t)1<<5);
  //RCC->APB2ENR = 1 |  (1<<3);  /*AFIO/ GPIOB*/
  //AFIO->MAPR = (AFIO->MAPR & ~((uint32_t)0x7 << 24)) | (2 << 24); /*释放 PA15 PB3 PB4*/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能
	//CS1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB8推挽输出  GPIO_Mode_AF_PP 片选CS1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB8
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	//CS2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB9推挽输出  GPIO_Mode_AF_PP 片选CS2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB8
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	//SO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;//PB4上拉输入 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB4
	
	//CLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB11复用推挽输出  GPIO_Mode_AF_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB3	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
}

u16 Max6675_readRawValue(uint8_t channel)
{
  u16 tmp=0;
  u16 i;
  if(channel==1)
  {
	MAX6675_CS_2_H;
	MAX6675_CS_L;//cs低
  }
	if(channel==2){
	MAX6675_CS_H;
	MAX6675_CS_2_L;
	}
  /*按位读取电平*/
  for(i=0;i<16;i++)
    {  
	/*创建一个沿，按沿读取PB4的端口电平*/
		MAX6675_CK_H;
		delay_us(10);
		if(MAX6675_SO)tmp|=0x0001;
		//printf("tmp%d---:%d\n",i,tmp);/*这是为了检查错误时按位打印每个数*/
		tmp<<=1;//按位左移
		MAX6675_CK_L;
		delay_us(10);
		}
	if(channel==1){
		MAX6675_CS_H;//cs高	
		}
	if(channel==2){
		MAX6675_CS_2_H;
	}  
   if (tmp&0X04) 
 {
    tmp = 0; //未检测到热电偶
	// printf("未检测到热电偶\n");//测试用
  }
  else 
 {
	tmp<<=1;//去掉D15位
	tmp>>=4;//去掉D0、1、2位
  }
	return tmp;
}
 
float Max6675_readTemperature(uint8_t channel)
{
	u16 d;
	float i,S;
	S=2.142857;//实验数，用温水和热水 环境测得的一个实际差量
	delay_ms(300);
	d=Max6675_readRawValue(channel);
	i=d*1023.75/4095/S;
    return i;
}

