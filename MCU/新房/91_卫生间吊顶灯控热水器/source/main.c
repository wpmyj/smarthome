/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 4MHz 外部晶振
其它：
人体红外PIR探头
继电器模块
光敏电阻

【修改历史】
日期            作者    备注
----------------------------------------------------------------------
2013年10月01日  黄长浩  初始版本
2014年01月27日  黄长浩  增加initDelay()
2014年10月03日  黄长浩  增加向92号节点发送数据的逻辑
                        升级NRF24L01+驱动
2014年10月06日  黄长浩  修改initDelay()延时为(NodeID*2)秒
					
【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "adc.h"
#include "stcEEPROM.h"

// Node ID
#define NODE_ID 91

sfr AUXR   = 0x8E;

sbit LIGHT_R = P1^4;  //光敏电阻 （10K上拉）
sbit PIR = P3^5;      //热释电红外传感器
sbit RELAY_HEATER = P3^7; //热水器继电器（30A）
sbit RELAY_LIGHT1 = P1^7; //灯控继电器1
sbit RELAY_LIGHT2 = P1^6; //灯控继电器2


volatile unsigned char light1Mode = 2; //0：常关，1：常开，2：自动
volatile unsigned char lightOnThreshold = 90; // 开灯的阈值

volatile unsigned char heaterMode = 0; //0：常关，非0：常开

volatile unsigned char timerCounter10ms = 0;
volatile unsigned int timerSendData = 0;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;



//开机延时 
//根据NodeID，进行约为(NodeID*2)秒的延时
//作用是避免所有节点同时上电，若都按5分钟间隔发送数据造成的通讯碰撞
void initDelay(void)
{
	//4MHz Crystal, 1T
    unsigned char a,b,c,d;
    for(d=NODE_ID;d>0;d--)
	    for(c=252;c>0;c--)
	        for(b=230;b>0;b--)
	            for(a=33;a>0;a--);
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 91, 231, 91}; //本节点的接收地址
	nrfSetRxMode( 92, 5, myAddr);
}


//返回当前亮度值
unsigned char getBrightness()
{
	return (255-getADCResult(4));
}


//将开灯阈值存入片内eeprom
void saveLightOnThreshold( unsigned char x )
{
	eepromEraseSector( 0x0000 );
	eepromWrite( 0x0000, x );
}


//从片内eeprom取得开灯阈值
unsigned char getLightOnThreshold()
{
	return eepromRead(0x0000);
}


//发送数据给Pi
void sendDataToHost( )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1] = 1; //Regular Status Update

	sendData[2] = PIR;
	sendData[3] = getBrightness(); //亮度
	sendData[4] = lightOnThreshold; 
		
	sendData[5] = ~RELAY_LIGHT1;
	sendData[6] = ~RELAY_LIGHT2;
	sendData[7] = ~RELAY_HEATER;

	tmp = nrfSendData( 96, 5, toAddr, 16, sendData);//Pi, 96频道，5字节地址，接收16字节
	
	//24L01开始接收数据
	startRecv(); 
}


//发送数据给92号节点
//供控制小厨宝用
void sendDataToNode92( unsigned char val )
{
	unsigned char sendData[3];
	unsigned char toAddr[5]= {97,83,92,231,92}; //Node 92地址
	unsigned char tmp;
	
	sendData[0]= NODE_ID;//Node ID
	sendData[1]= val;

	tmp = nrfSendData( 92, 5, toAddr, 3, sendData);//Pi, 92频道，5字节地址，接收3字节
	
	//24L01开始接收数据
	startRecv(); 
}


// 初始化Timer0
void initTimer0(void)
{
	AUXR = AUXR|0x80;  // T0, 1T Mode
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}


void initRelays()
{
	RELAY_LIGHT1=1;
	RELAY_LIGHT2=1;
	RELAY_HEATER=1;
}


void main()
{

	bit lastPIR=0;
	bit thisPIR=0;
	
	//初始化中断0
	initINT0();
    
	//初始化继电器
	initRelays();
	
	//初始化ADC
	initADC();
	
	//取得开灯阈值
	lightOnThreshold = getLightOnThreshold();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
	
	//初始化延时
	initDelay();
	
	//初始化timer0
	initTimer0();

	while(1)
	{
		//取得传感器的当前值
		thisPIR = PIR;

		//Send data to Pi
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost();
		}
		
		// 灯控1
		if( light1Mode == 0 ) //常关
		{
			RELAY_LIGHT1=1;
		}
		else if( light1Mode ==1 ) //常开
		{
			RELAY_LIGHT1=0;
		}
		else if( light1Mode ==2 ) //自动
		{
			if(RELAY_LIGHT1) //当前灯是灭的
			{
				if( thisPIR && getBrightness()<=lightOnThreshold ) //有人，且环境亮度在阈值以下
				{
					RELAY_LIGHT1 = 0; //开灯
				}
				
			}
			else //当前灯是亮的
			{
				RELAY_LIGHT1 = ~thisPIR; //继续亮，或关灯
			}
		}
		
		//控制小厨宝（NodeID:92）
		//当有人无人状态发生转变时，将数据传给92号节点
		if( thisPIR != lastPIR ) 
		{
			sendDataToNode92( thisPIR );
		}
		
		//记录当前值
		lastPIR = thisPIR;

	}
}


//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char tmp;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): From Node ID, 固定为1
	// *(receivedData+1): 开灯的亮度阈值
	// *(receivedData+2): 灯控继电器1的工作模式
	// *(receivedData+3): 灯控继电器2的工作模式
	// *(receivedData+4): 热水器继电器工作模式
	
	//开灯阈值
	tmp = *(receivedData+1);
	if( tmp!= lightOnThreshold )
	{
		//threshold changed, let's save it.
		lightOnThreshold = tmp;
		saveLightOnThreshold( lightOnThreshold );
	}
	
	light1Mode = *(receivedData+2);
	
	RELAY_HEATER = (*(receivedData+4))==0?1:0;
}


//定时器0中断处理程序
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
    
	if( ++timerCounter10ms == 100 ) //100个10ms，即1秒
	{
		timerCounter10ms=0;
		
		if( ++timerSendData == 600 ) //每600秒向Pi发送一次数据
		{
			timerSendData = 0;
			sendDataNow = 1;
		}
		

	}
}

