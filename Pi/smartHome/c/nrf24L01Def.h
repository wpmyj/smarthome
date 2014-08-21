

#ifndef __NRF24L01DEF_H__
#define __NRF24L01DEF_H__

/******************************/
#define  R_REGISTER      0x00
#define  W_REGISTER      0x20
#define  R_RX_PAYLOAD 	 0x61
#define  W_TX_PAYLOAD	 0xa0
#define  FLUSH_TX		 0xe1
#define  FLUSH_RX		 0xe2
#define  REUSE_TX_PL     0xe3
#define  NOP             0xff
#define  W_TX_PAYLOAD_NOACK 0xB0	// Used in TX mode, Disable AUTOACK on this specific packet

/******************寄存器地址****************************/
#define  CONFIG          0x00//配置寄存器
#define  EN_AA			 0x01//使能自动应答
#define  EN_RXADDR       0x02//接收通道使能0-5个通道
#define  SETUP_AW        0x03//设置数据通道地址宽度3-5
#define  SETUP_RETR      0x04//建立自动重发
#define  RF_CH           0x05//射频通道设置
#define  RF_SETUP        0x06//射频寄存器
#define  STATUS          0x07//状态寄存器
#define  OBSERVE_TX      0x08//发送检测寄存器
#define  CD              0x09//载波
#define  RX_ADDR_P0      0x0a//数据通道0接收地址
#define  RX_ADDR_P1      0x0b//数据通道1接收地址
#define  RX_ADDR_P2      0x0c//数据通道2接收地址
#define  RX_ADDR_P3      0x0d//数据通道3接收地址
#define  RX_ADDR_P4      0x0e//数据通道4接收地址
#define  RX_ADDR_P5      0x0f//数据通道5接收地址
#define  TX_ADDR         0x10//发送地址
#define  RX_PW_P0        0x11//P0通道数据宽度设置
#define  RX_PW_P1        0x12//P1通道数据宽度设置
#define  RX_PW_P2        0x13//P2通道数据宽度设置
#define  RX_PW_P3        0x14//P3通道数据宽度设置
#define  RX_PW_P4        0x15//P4通道数据宽度设置
#define  RX_PW_P5        0x16//P5通道数据宽度设置
#define  FIFO_STATUS     0x17//FIFO状态寄存器
#define  FEATURE         0x1D// Additional features register, needed to enable the additional commands

//本节点的接收的数据宽度 (1-32字节)
#define RECEIVE_DATA_WIDTH 16

/**
 * Define Hardware wiring
 *
 *    NRF24L01+      RPi引脚号     WiringPi
 *----------------------------------------
 *   GND PIN 1 ------ PIN 6  ------ N.A.
 *  3.3v PIN 2 ------ PIN 1  ------ N.A.
 *    CE PIN 3 ------ PIN 7  ------ 7
 *   CSN PIN 4 ------ PIN 13 ------ 2
 *  SCLK PIN 5 ------ PIN 15 ------ 3
 *  MOSI PIN 6 ------ PIN 19 ------ 12
 *  MISO PIN 7 ------ PIN 21 ------ 13
 *   IRQ PIN 8 ------ PIN 23 ------ 14
 **/
#define CE 7
#define CSN 2
#define SCLK 3
#define MOSI 12
#define MISO 13
#define IRQ 14

#endif
