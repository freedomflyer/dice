//	RBX430_lcd.c
//******************************************************************************
//******************************************************************************
//	LL        CCCCC     DDCDDD
//	LL       CC   CC    DD   DD
//	LL      CC          DD    DD
//	LL      CC          DD    DD
//	LL      CC          DD    DD
//	LL       CC   CC    DD   DD
//	LLLLLL    CCCCC     DDDDDD
//******************************************************************************
//******************************************************************************
//	Author:			Paul Roper
//	Revision:		1.0		03/05/2012	RBX430-1
//					1.1		divu8, image1, image2
//					1.2	09/17/2012 fill fixes
//
//	Description:	Controller firmware for YM160160C/ST7529 LCD
//
//	Built with CCSv5.2 w/cgt 3.0.0
//******************************************************************************
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "msp430x22x4.h"
#include "RBX430-1.h"
#include "RBX430_lcd.h"

static uint16 lcd_dmode;			// lcd mode
static uint8 lcd_y;					// row (0-159)
static uint8 lcd_x;					// column (0-159)

extern uint16 i2c_fSCL;				// i2c timing constant

#define BYU_LOGO	1				// include BYU logo
//
//******************************************************************************
//******************************************************************************
// constant image - BYU
//
#if BYU_LOGO
const uint8 byu_image[] = { 91, 24,		// 91 wide, 24 height
//           . 1      .   2    .     3  .       4.        . 5      .   6    .     7  .       8.          9         1
//   12345678.90123456.78901234.56789012.34567890.12345678.90123456.78901234.56789012.34567890.12345678.90123456|890
// 0         .        .        . OOOOOOO.OOOOOOOO.        . OOOOOOO.OOOOOOOO.        .        .        .   |
// 1         .        .        . OOOOOOO.OOOOOOOO.        . OOOOOOO.OOOOOOOO.        .        .        .   |
// 2         .        .        .    OOOO.OOOOOO  .        .   OOOOO.OOOOO   .        .        .        .   |
// 3         .        .        .     OOO.OOOOOOO .        .  OOOOOO.OOOO    .        .        .        .   |
// 4         .        .        .      OO.OOOOOOOO.        . OOOOOOO.OOO     .        .        .        .   |
// 5 OOOOOOOO.OOOOOOOO.OOOOOOOO.       O.OOOOOOOO.O       .OOOOOOOO.OO   OOO.OOOOOOOO.O      O.OOOOOOOO.OOO|
// 6 OOOOOOOO.OOOOOOOO.OOOOOOOO.OO      .OOOOOOOO.OO     O.OOOOOOOO.O    OOO.OOOOOOOO.O      O.OOOOOOOO.OOO|
// 7  OOOOOOO.OOOOOOOO.OOOOOOOO.OOO     . OOOOOOO.OOO   OO.OOOOOOOO.      OO.OOOOOOOO.        .OOOOOOOO.OO |
	0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x03, 0xc3, 0xc1, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc,
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xcf,
	0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03,
	0x07, 0xcf, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc6, 0xc7, 0xc7,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x06,
// 0  OOOOOOO.OOO     .  OOOOOO.OOOO    .  OOOOOO.OOOO OOO.OOOOOOO .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 1  OOOOOOO.OOO     .   OOOOO.OOOOO   .   OOOOO.OOOOOOOO.OOOOOO  .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 2  OOOOOOO.OOO     .   OOOOO.OOOOO   .    OOOO.OOOOOOOO.OOOOO   .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 3  OOOOOOO.OOO     .   OOOOO.OOOOO   .     OOO.OOOOOOOO.OOOO    .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 4  OOOOOOO.OOO     .  OOOOOO.OOOO    .      OO.OOOOOOOO.OOO     .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 5  OOOOOOO.OOOOOOOO.OOOOOOOO.OOO     .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 6  OOOOOOO.OOOOOOOO.OOOOOOOO.OO      .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 7  OOOOOOO.OOOOOOOO.OOOOOOOO.OOO     .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xfd, 0xf8, 0x70, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x00,
// 0  OOOOOOO.OOO     .  OOOOOO.OOOO    .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 1  OOOOOOO.OOO     .   OOOOO.OOOOO   .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 2  OOOOOOO.OOO     .   OOOOO.OOOOO   .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 3  OOOOOOO.OOO     .   OOOOO.OOOOO   .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.        .OOOOOOOO.OO |
// 4  OOOOOOO.OOO     .  OOOOOO.OOOO    .       O.OOOOOOOO.OO      .      OO.OOOOOOOO.O      O.OOOOOOOO.OO |
// 5  OOOOOOO.OOOOOOOO.OOOOOOOO.OOO     .       O.OOOOOOOO.OO      .       O.OOOOOOOO.OOOOOOOO.OOOOOOOO.O  |
// 6 OOOOOOOO.OOOOOOOO.OOOOOOOO.OO      .      OO.OOOOOOOO.OOO     .        .OOOOOOOO.OOOOOOOO.OOOOOOOO.   |
// 7 OOOOOOOO.OOOOOOOO.OOOOOOOO.        .      OO.OOOOOOOO.OOO     .        . OOOOOOO.OOOOOOOO.OOOOOOO .   |
	0x03, 0xff, 0xff, 0xff, 0xff, 0xff,	0xff, 0xff,
	0xff, 0xff, 0xff, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xfe, 0xfe, 0xfc, 0xf8, 0x70, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfc,
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0f,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
	0xfc, 0xf8, 0x00
};

const uint8 byu_image1[] = { 64, 40,			// 64 wide, 40 height
	0x00,0x00,0x00,0x3f,0xfe,0x00,0x00,0x00,	//	-------- -------- -------- --OOOOOO OOOOOOO- -------- -------- --------
	0x00,0x00,0x07,0xff,0xff,0xf0,0x00,0x00,	//	-------- -------- -----OOO OOOOOOOO OOOOOOOO OOOO---- -------- --------
	0x00,0x00,0x7f,0xff,0xff,0xfe,0x00,0x00,	//	-------- -------- -OOOOOOO OOOOOOOO OOOOOOOO OOOOOOO- -------- --------
	0x00,0x01,0xff,0x87,0xff,0xff,0x80,0x00,	//	-------- -------O OOOOOOOO O----OOO OOOOOOOO OOOOOOOO O------- --------
	0x00,0x07,0xf8,0x03,0xff,0x1f,0xe0,0x00,	//	-------- -----OOO OOOOO--- ------OO OOOOOOOO ---OOOOO OOO----- --------
	0x00,0x1f,0x80,0x03,0xfe,0x03,0xf8,0x00,	//	-------- ---OOOOO O------- ------OO OOOOOOO- ------OO OOOOO--- --------
	0x00,0x7e,0x00,0x01,0xfc,0x00,0x7e,0x00,	//	-------- -OOOOOO- -------- -------O OOOOOO-- -------- -OOOOOO- --------
	0x00,0xf8,0x00,0x01,0xfc,0x00,0x1f,0x00,	//	-------- OOOOO--- -------- -------O OOOOOO-- -------- ---OOOOO --------
	0x01,0xe0,0x00,0x01,0xff,0x00,0x0f,0x80,	//	-------O OOO----- -------- -------O OOOOOOOO -------- ----OOOO O-------
	0x03,0xc0,0x00,0x0f,0xff,0xe0,0x03,0xc0,	//	------OO OO------ -------- ----OOOO OOOOOOOO OOO----- ------OO OO------
	0x07,0x80,0x00,0x7f,0xff,0xf0,0x01,0xe0,	//	-----OOO O------- -------- -OOOOOOO OOOOOOOO OOOO---- -------O OOO-----
	0x0f,0x00,0x00,0x3f,0xff,0xe0,0x00,0xf0,	//	----OOOO -------- -------- --OOOOOO OOOOOOOO OOO----- -------- OOOO----
	0x1f,0x00,0x00,0x1f,0xff,0xc0,0x01,0xf8,	//	---OOOOO -------- -------- ---OOOOO OOOOOOOO OO------ -------O OOOOO---
	0x1f,0x81,0x00,0x0f,0xff,0x80,0x43,0xf8,	//	---OOOOO O------O -------- ----OOOO OOOOOOOO O------- -O----OO OOOOO---
	0x3f,0xc7,0x80,0x07,0xff,0x00,0xf7,0xfc,	//	--OOOOOO OO---OOO O------- -----OOO OOOOOOOO -------- OOOO-OOO OOOOOO--
	0x3f,0xdf,0xc0,0x03,0xfe,0x01,0xff,0xfc,	//	--OOOOOO OOO-OOOO OO------ ------OO OOOOOOO- -------O OOOOOOOO OOOOOO--
	0x3f,0xff,0xe0,0x01,0xfc,0x03,0xff,0xfc,	//	--OOOOOO OOOOOOOO OOO----- -------O OOOOOO-- ------OO OOOOOOOO OOOOOO--
	0x7f,0xff,0xf0,0x00,0xf8,0x07,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOO---- -------- OOOOO--- -----OOO OOOOOOOO OOOOOOO-
	0x7f,0xff,0xf8,0x00,0x70,0x0f,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOOO--- -------- -OOO---- ----OOOO OOOOOOOO OOOOOOO-
	0x7f,0xff,0xfc,0x00,0x20,0x1f,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOOOO-- -------- --O----- ---OOOOO OOOOOOOO OOOOOOO-
	0x7f,0xff,0xfe,0x00,0x00,0x3f,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOOOOO- -------- -------- --OOOOOO OOOOOOOO OOOOOOO-
	0x7f,0xff,0xff,0x00,0x00,0x7f,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOOOOOO -------- -------- -OOOOOOO OOOOOOOO OOOOOOO-
	0x7f,0xff,0xff,0x80,0x00,0xff,0xff,0xfe,	//	-OOOOOOO OOOOOOOO OOOOOOOO O------- -------- OOOOOOOO OOOOOOOO OOOOOOO-
	0x3f,0xff,0xff,0xc0,0x01,0xff,0xff,0xfe,	//	--OOOOOO OOOOOOOO OOOOOOOO OO------ -------O OOOOOOOO OOOOOOOO OOOOOOO-
	0x3f,0xff,0xff,0xe0,0x03,0xff,0xff,0xfc,	//	--OOOOOO OOOOOOOO OOOOOOOO OOO----- ------OO OOOOOOOO OOOOOOOO OOOOOO--
	0x3f,0xff,0xff,0xe0,0x07,0xff,0xff,0xfc,	//	--OOOOOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOOOOO--
	0x1f,0xff,0xff,0xe0,0x07,0xff,0xff,0xf8,	//	---OOOOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOOOO---
	0x1f,0xff,0xff,0xe0,0x07,0xff,0xff,0xf8,	//	---OOOOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOOOO---
	0x0f,0xff,0xff,0xe0,0x07,0xff,0xff,0xf0,	//	----OOOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOOO----
	0x07,0xff,0xff,0xe0,0x07,0xff,0xff,0xf0,	//	-----OOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOOO----
	0x07,0xff,0xff,0xe0,0x07,0xff,0xff,0xe0,	//	-----OOO OOOOOOOO OOOOOOOO OOO----- -----OOO OOOOOOOO OOOOOOOO OOO-----
	0x01,0xff,0xf8,0xe0,0x07,0x8f,0xff,0xc0,	//	-------O OOOOOOOO OOOOO--- OOO----- -----OOO O---OOOO OOOOOOOO OO------
	0x00,0xff,0xf8,0x00,0x00,0x0f,0xff,0x00,	//	-------- OOOOOOOO OOOOO--- -------- -------- ----OOOO OOOOOOOO --------
	0x00,0x7f,0xf8,0x00,0x00,0x0f,0xfe,0x00,	//	-------- -OOOOOOO OOOOO--- -------- -------- ----OOOO OOOOOOO- --------
	0x00,0x3f,0xf8,0x00,0x00,0x0f,0xfc,0x00,	//	-------- --OOOOOO OOOOO--- -------- -------- ----OOOO OOOOOO-- --------
	0x00,0x0f,0xfc,0x00,0x00,0x1f,0xf0,0x00,	//	-------- ----OOOO OOOOOO-- -------- -------- ---OOOOO OOOO---- --------
	0x00,0x03,0xfc,0x00,0x00,0x1f,0xc0,0x00,	//	-------- ------OO OOOOOO-- -------- -------- ---OOOOO OO------ --------
	0x00,0x00,0xff,0xe0,0x03,0xff,0x00,0x00,	//	-------- -------- OOOOOOOO OOO----- ------OO OOOOOOOO -------- --------
	0x00,0x00,0x1f,0xff,0xff,0xf8,0x00,0x00,	//	-------- -------- ---OOOOO OOOOOOOO OOOOOOOO OOOOO--- -------- --------
	0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00		//	-------- -------- -------- OOOOOOOO OOOOOOOO -------- -------- --------
};
#endif


//******************************************************************************
//******************************************************************************
//	Sitronix ST7529 controller functions
//
//	void WriteCmd(uint8 c)
//	int ReadData(void)
//	void WriteData(uint8 c)
//	void WriteData_word(uint16 w)
//
//		A0	RW	A0 + ~RW	Function
//		--	--	--------	-----------------
//		0	0	   1		Control Write
//		0	1	   0		Control Read (Reset)
//		1	0      1		Display Write
//		1	1	   1		Display Read
//
//
void WriteCmd(uint8 c)
{
	P2DIR = 0xff;		// output to P2
	P2OUT = c;			// set data on output lines
	LCD_RW_L;			// set RW low (write)
	LCD_A0_L;			// set A0 low (command)
//	LCD_CS_L;			// drop CS
	LCD_E_H;			// toggle E
	LCD_E_L;
//	LCD_CS_H;			// raise CS
	return;
} // end WriteCmd


int ReadData(void)
{
	int data;

	P2DIR = 0x00;		// input from P2
	LCD_A0_H;			// set A0 high (data)
	LCD_RW_H;			// set RW high (read)
//	LCD_CS_L;			// drop CS
	LCD_E_H;			// toggle E
	_no_operation();	// nop
	data = P2IN;		// read data
	LCD_E_L;
//	LCD_CS_H;			// raise CS
	return data;
} // end ReadData


void WriteData(uint8 c)
{
	P2DIR = 0xff;		// output to P2
	P2OUT = c;			// set data on output lines
	LCD_RW_L;			// set RW low (write)
	LCD_A0_H;			// set A0 high (data)
//	LCD_CS_L;			// drop CS
	LCD_E_H;			// toggle E
	LCD_E_L;
//	LCD_CS_H;			// raise CS
	return;
} // end WriteData


void WriteData_word(uint16 data)
{
	P2DIR = 0xff;		// output to P2
	P2OUT = data >> 8;	// set data on output lines
	LCD_RW_L;			// set RW low (write)
	LCD_A0_H;			// set A0 high (data)
	LCD_E_H;			// toggle E
	LCD_E_L;

	_no_operation();	// nop
	P2OUT = data;		// set data on output lines
	LCD_E_H;			// toggle E
	LCD_E_L;
	return;
} // end WriteData_word


//******************************************************************************
//	Function: void DelayMs(WORD time)
//
//	PreCondition: none
//	Input: time - delay in ms
//	Output: none
//	Side Effects: none
//	Overview: delays execution on time specified in ms
//
//	Note: none
//
//******************************************************************************

#define DELAY_1MS	1000
void DelayMs(uint16 time)
{ 
	uint16 delay;
	uint16 delay_1m = i2c_fSCL * DELAY_1MS;

	while(time--) 
	for(delay = 0; delay < delay_1m; delay++);
	return; 
} // end DelayMs


//******************************************************************************
//	Function: void lcd_init()
//
//	PreCondition: none
//	Input: none
//	Output: none
//	Side Effects: none
//	Overview: resets LCD, initializes PMP
//
//	Note: Sitronix	ST7529 controller drive
//					1/160 Duty, 1/13 Bias
//
//******************************************************************************

#define Ra_Rb	0x27
#define vopcode	335				// vop = 14.0v
#define LCD_DELAY	50

uint8 lcd_init(void)
{ 
	LCD_RW_H;					// set RW high (read)
	LCD_A0_H;					// set A0 high (data)
	DelayMs(LCD_DELAY); 

	// Hold in reset 
	LCD_A0_L;					// set A0 low (command) RESET
	DelayMs(LCD_DELAY); 

	// Release from reset 
	LCD_A0_H;					// set A0 high (data)
	DelayMs(LCD_DELAY); 

	WriteCmd(0x30);				// Ext = 0 

	WriteCmd(0x94);				// Sleep Out 

	WriteCmd(0xd1);				// OSC On 
	WriteCmd(0x20);				// Power Control Set 
		WriteData(0x08);		// Booster Must Be On First 
	DelayMs(2); 

	WriteCmd(0x20);				// Power Control Set 
		WriteData(0x0b);		// Booster, Regulator, Follower ON 

	WriteCmd(0x81);				// Electronic Control
		WriteData(vopcode & 0x3f);
		WriteData(vopcode >> 6);
 
	WriteCmd(0xca);				// Display Control 
		WriteData(0x00);		// CLD=0 
		WriteData(0x27);		// Duty=(160/4-1)=39 
		WriteData(0x00);		// FR Inverse-Set Value ???

	WriteCmd(0xa6);				// Normal Display 

	WriteCmd(0xbb);				// COM Scan Direction 
		WriteData(0x01);		// 0->79 159->80

	WriteCmd(0xbc);				// Data Scan Direction
		WriteData(0x01);		// CI=0, LI=1
		WriteData(0x01);		// CLR=1 (P3/P2/P1)
		WriteData(0x01);		// 2B3P

	WriteCmd(0x31);				// Ext = 1

	WriteCmd(0x20);				// set gray 1 values
		WriteData(0x00);		// 0
		WriteData(0x03);		// 1
		WriteData(0x06);		// 2
		WriteData(0x09);		// 3
		WriteData(0x0b);		// 4
		WriteData(0x0d);		// 5
		WriteData(0x0e);		// 6
		WriteData(0x0f);		// 7
		WriteData(0x10);		// 8
		WriteData(0x11);		// 9
		WriteData(0x12);		// 10
		WriteData(0x14);		// 11
		WriteData(0x16);		// 12
		WriteData(0x18);		// 13
		WriteData(0x1b);		// 14
		WriteData(0x1f);		// 15

	WriteCmd(0x21);				// set gray 2 values
		WriteData(0x00);		// 0
		WriteData(0x03);		// 1
		WriteData(0x06);		// 2
		WriteData(0x09);		// 3
		WriteData(0x0b);		// 4
		WriteData(0x0d);		// 5
		WriteData(0x0e);		// 6
		WriteData(0x0f);		// 7
		WriteData(0x10);		// 8
		WriteData(0x11);		// 9
		WriteData(0x12);		// 10
		WriteData(0x14);		// 11
		WriteData(0x16);		// 12
		WriteData(0x18);		// 13
		WriteData(0x1b);		// 14
		WriteData(0x1f);		// 15

	WriteCmd(0x32);				// Analog Circuit Set 
		WriteData(0x00);		// OSC Frequency =000 (Default) 
		WriteData(0x01);		// Booster Efficiency=01(Default) 
		WriteData(0x01);		// Bias=1/13

	WriteCmd(0x34);				// Software init

	WriteCmd(0x30);				// Ext = 0 
	WriteCmd(0xaf);				// Display On 

	lcd_dmode = 0;
	lcd_y = HD_Y_MAX - 1;
	lcd_x = 0;				// column (0-159)
	return 0;
} // end  lcd_init


//******************************************************************************
//	fast uint8 / 3
//
unsigned divu3(unsigned n)
{
	unsigned q, r, t;
//	q = (n >> 2) + (n >> 4);		// q = n*0.0101 (approx).
	q = (n >> 2);					// q = n*0.0101 (approx).
	q += (q >> 2);

//	q += (q >> 4);					// q = n*0.01010101.
	t = (q >> 2);
	q += (t >> 2);

//	q += (q >> 8);					// (not needed for uint8/3)

	r = n - q * 3;					// 0 <= r <= 15.

//	return q + ((11 * r) >> 5);		// Returning q + r/3.
	t = r << 3;
	return q + ((r + r + r + t) >> 5);
} // end divu3


//******************************************************************************
//	set lcd x, y
//
void lcd_set_x_y(uint8 x, uint8 y)
{
	WriteCmd(0x75);					// set line address
		WriteData(y);				// from line 0 - 159
		WriteData(0x9f);

	WriteCmd(0x15);					// set column address
		WriteData(divu3(x));		// from col 0 - 160/3
		WriteData(0x35);
	return;
} // end lcd_set_x_y


//******************************************************************************
//	lcd read word
//
uint16 lcd_read_word(int16 column, int16 row)
{
	WriteCmd(0x75);					// set line address
		WriteData(row);				// from line 0 - 159
		WriteData(0x9f);

	WriteCmd(0x15);					// set column address
		WriteData(column);			// from col 0 - 160
		WriteData(0x35);
	WriteCmd(0x5d);					// RAMRD - read from memory
	ReadData();						// Dummy read
	return (ReadData() << 8) + ReadData();
} // end lcd_read_word


//******************************************************************************
//	lcd write word
//
void lcd_write_word(int16 column, int16 row, uint16 data)
{
	WriteCmd(0x75);					// set line address
		WriteData(row);				// from line 0 - 159
		WriteData(0x9f);

	WriteCmd(0x15);					// set column address
		WriteData(column);			// from col 0 - 160
		WriteData(0x35);
	WriteCmd(0x5c);					// RAMWR - write to memory
	WriteData(data >> 8);			// write high byte
	WriteData(data & 0x00ff);		// write low byte
	return;
} // end lcd_write_word


//******************************************************************************
//	clear lcd screen
//
void lcd_clear()
{
	lcd_set(0xffdf);				// clear lcd
}


//******************************************************************************
//	clear lcd screen
//
void lcd_set(uint16 value)
{ 
	int i; 

	lcd_set_x_y(0, 0);			// upper right corner
	WriteCmd(0x5c);				// start write

	// whole screen - rows x columns
	for(i = 0; i < 160 * (divu3((160*2))); i++)
	{
		WriteData_word(value);
	} 
	lcd_dmode = 0;				// reset mode
	lcd_y = HD_Y_MAX - 1;		// upper left hand corner
	lcd_x = 0;
	return;
} // end  lcd_clear


//******************************************************************************
//	Display Image
//
//	uint8 lcd_image(const uint8* image, int16 x, int16 y)
//	uint8 lcd_image1(const uint8* image, int16 x, int16 y, uint8 flag)
//	uint8 lcd_image2(const uint8* image, int16 x, int16 y, uint8 flag)
//
//	IN:		const char* image	pointer to image
//
//			bit image preceded by width/height coordinates
//
uint8 lcd_image(const uint8* image, int16 x, int16 y)
{
	int16 x1, y1, data, mask;
	int16 right = x + *image++;			// stop at right side of image
	int16 bottom = y;					// finish at bottom
	y += *image++;						// get top of image

	while (y > bottom)					// display from top down
	{
		for (x1 = x; x1 < right; x1++)	// display from left to right
		{
			data = *image++;			// get image byte
			y1 = y;
			for (mask = 0x80; mask; mask >>= 1)
			{
				if (data & mask) lcd_point(x1, --y1, 1);
				else if (1) lcd_point(x1, --y1, 0);
			}
		}
		y -= 8;
	}	
	return 0;
} // end lcd_image


//******************************************************************************
//	output LCD ram image
//	flag = 1	output LCD RAM image
//	       0	blank image
//	       2	fill image area
//
//	x needs to be divisible by 3
//
uint8 lcd_image1(const uint8* image, int16 x, int16 y, uint8 flag)
{
	int16 i, data, index;
	uint8 bits, mask;
	int16 width = *image++;				// get width/height
	int16 height = *image++;
	int16 bottom = y;

	x += width - 1;						// move to top, left (make 0 based)
	y += height;

	while (y > bottom)					// display from top down
	{
		lcd_set_x_y(159 - x, y);		// upper right corner
		WriteCmd(0x5c);					// write to memory

		data = 0x0000;					// fill
		switch (flag)
		{
			case 0:
				data = 0xffdf;			// erase

			case 2:
			{
				for (i = width; i > 0; i -= 3)	// display from right to left
				{
					WriteData_word(data);
				}
			}
			break;

			default:
			case 1:
			{
				image += (width >> 3);		// point to end of image line
				mask = 0x80;
				data = 0xffdf;				// assume all off
				index = 0;

				for (i = width; i > 0; i--)		// display from right to left
				{
					mask <<= 1;					// adjust mask
					if (mask == 0)
					{
						mask = 0x01;			// reset mask
						bits = *--image;		// get next data byte
					}
					if (bits & mask)
					{
						if (index == 0) data &= 0xffe0;
						else if (index == 1) data &= 0xf83f;
						else data &= 0x07df;
					}
					if (++index == 3)
					{
						WriteData_word(data);
						data = 0xffdf;			// assume all off
						index = 0;
					}
				}
				if (index) WriteData_word(data);	// flush data
				image += (width >> 3);			// point to end of image line
			}
			break;
		}
		y--;
	}
	return 0;
} // end lcd_image1


//******************************************************************************
//	output LCD ram image
//	flag = 1	output LCD RAM image
//	       0	blank image
//	       2	fill image area
//
//	x needs to be divisible by 3
//
uint8 lcd_image2(const uint8* image, int16 x, int16 y, uint8 flag)
{
	int16 x1;
	int16 width = *image++;				// get width/height
	int16 height = *image++;
	int16 bottom = y;

	x += width - 1;						// move to top, left (make 0 based)
	y += height;
	width = divu3(((width * 2) + 2));	// 3 pixels per 2 bytes (round up)

	while (y > bottom)					// display from top down
	{
		lcd_set_x_y(159 - x, y);		// upper right corner
		WriteCmd(0x5c);					// write to memory

		for (x1 = width; x1 > 0; x1 -= 2)		// display from right to left
		{
			switch (flag)
			{
				case 0:
					WriteData_word(0xffdf);	// erase
					break;

				case 1:
					WriteData(image[x1 - 1]);	// ~(0Xf8*P2 | 0x07*P1)
					WriteData(image[x1 - 2]);	// ~(0xc0*P1 | 0x1f*P0)
					break;

				default:
				case 2:
					WriteData_word(0x0000);	// fill
					break;
			}
		}
		image += width;					// next line
		y--;
	}
	return 0;
} // end lcd_image2


//******************************************************************************
//	Fill Image
//
//	IN:		x, y			lower right coordinates
//			width,height	area to blank
//			flag = 0	blank image
//	    		   1	(unused)
//	    		   2	fill image area
//
uint8 lcd_fill(int16 x, int16 y, uint16 width, uint16 height, uint8 flag)
{
	uint8 area[2];
	area[0] = width;
	area[1] = height;
	return lcd_image2(area, x, y, flag);
}


//******************************************************************************
//	Blank Image
//
//	IN:		x, y			lower right coordinates
//			width,height	area to blank
//
uint8 lcd_blank(int16 x, int16 y, uint16 width, uint16 height)
{
	int16 i, j;

	for (i = x; i < x + width; i++)
	{
		for (j = y; j < y + height; j++)
		{
			lcd_point(i, j, 0);
		}
	}
	return 0;
} // end lcd_blank


//******************************************************************************
//	change lcd volume (brightness)
//
void lcd_volume(uint16 volume)
{
	WriteCmd(0x81);			// Electronic Control
		WriteData(volume & 0x3f);
		WriteData(volume >> 6);
	return;
} // end lcd_volume


//******************************************************************************
//	Turn ON/OFF LCD backlight
//
void lcd_backlight(uint8 backlight)
{
	if (backlight)
	{
		BACKLIGHT_ON;					// turn on backlight
	}
	else
	{
		BACKLIGHT_OFF;					// turn off backlight
	}
	return;
} // end lcd_backlight


//******************************************************************************
//	Display Mode
//
//	xxxx xxxx xxxx xxxx
//	           \\\\ \\\\___ LCD_PROPORTIONAL		proportional font
//	            \\\\ \\\___ LCD_REVERSE_FONT		reverse font
//	             \\\\ \\___ LCD_2X_FONT				2x font
//	              \\\\ \___ LCD_FRAM_CHARACTER		write to FRAM
//	               \\\\____ LCD_REVERSE_DISPLAY		reverse display
//	                \\\____
//		             \\____
//	                  \____
//
//	~mode = Turn OFF mode bit(s)
//
uint16 lcd_mode(int16 mode)
{
	if (mode)
	{
		// set/reset mode bits
		if (mode > 0) lcd_dmode |= mode;		// set mode bits
		else lcd_dmode &= mode;				// reset mode bits
	}
	else
	{
		lcd_dmode = 0;
	}
	return lcd_dmode;
} // end lcd_mode


//******************************************************************************
//	access lcd point at x,y
//
//	flag	0 = turn single point off
//			1 = turn single point on
//			2 = turn double point off
//			3 = turn double point on
//		   -1 = read point (0 or 1)
//
//	pen =	0000 0000
//	         \\\\ \\\\
//	          \\\\ \\\\_ 0=erase, 1=draw
//	           \\\\ \\\_ 0=single, 1=double
//	            \\\\ \\_ 0=no fill, 1=fill
//
//	return results
//
uint8 lcd_point(int16 x, int16 y, int16 flag)
{
	uint8 pixel1, pixel2;

	// return 1 if out of range
	if ((x < 0) || (x >= HD_X_MAX)) return 1;
	if ((y < 0) || (y >= HD_Y_MAX)) return 1;

	if (flag < 0)
	{
		flag = 4;
	}
	else
	{
		flag &= 0x03;
		switch (flag)
		{
			case 2:					// double point off
				lcd_point(x-1, y, OFF);
				lcd_point(x, y+1, OFF);
				lcd_point(x+1, y, OFF);
				lcd_point(x, y-1, OFF);
				lcd_point(x, y, OFF);
				return 0;

			case 3:					// double point on
				lcd_point(x-1, y, ON);
				lcd_point(x, y+1, ON);
				lcd_point(x+1, y, ON);
				lcd_point(x, y-1, ON);
				lcd_point(x, y, ON);
				return 0;

			default:					// mask flag to ON or OFF
				break;
		}
	}

	// translate point
	x = 159 - x;
//	y = 159 - y;

	lcd_set_x_y(x, y);		// upper right corner

	// read point
	WriteCmd(0xe0);				// RMWIN - read and modify write
	ReadData();					// Dummy read
	pixel1 = ReadData();		// Start read cycle for pixel 2/1
	pixel2 = ReadData();		// Start read cycle for pixel 1/0

	// process point
	switch (flag)
	{
		case 0:					// turn point off
//			switch (x % 3)
			switch (x - divu3(x) * 3)
			{
				case 0:
					pixel2 |= 0x1f;
					break;

				case 1:
					pixel1 |= 0x07;
					pixel2 |= 0xc0;
					break;

				case 2:
				default:
					pixel1 |= 0xf8;
			}
			break;

		case 1:					// turn point on
//			switch (x % 3)
			switch (x - divu3(x) * 3)
			{
				case 0:
					pixel2 &= 0xc0;
					break;

				case 1:
					pixel1 &= 0xf8;
					pixel2 &= 0x1f;
					break;

				case 2:
				default:
					pixel1 &= 0x07;
			}
			break;

		default:
		case 4:					// read point
//			switch (x % 3)
			switch (x - divu3(x) * 3)
			{
				case 0:
					return (pixel2 & 0x1f) ? 1 : 0;

				case 1:
					return ((pixel1 & 0x07) && (pixel2 & 0xc0)) ? 1 : 0;

				case 2:
				default:
					return (pixel1 & 0xf8) ? 1 : 0;
			}
	}
	// Write pixels back
	WriteData(pixel1);
	WriteData(pixel2);

	WriteCmd(0xee);				// RMWOUT - cancel read modify write mode
	return 0;					// return success
} // end lcd_point


//******************************************************************************
//	draw circle of radius r0 and center x0,y0
//
void lcd_circle(int16 x0, int16 y0, uint16 r0, uint8 pen)
{
	int16 x, y, d;
	int16 i, j;

	x = x0;
	y = y0 + r0;
	d =  3 - r0 * 2;

	do
	{
		if (pen & 0x04)
		{
			for (i = x0 - (x - x0); i <= x; i++)
    		{
        		lcd_point(i, y, pen);
    			lcd_point(i,  y0 - (y - y0), pen);
    		}
    		for (j = y0 - (x - x0); j <= y0 + (x - x0) ; j++)
    		{
    			for (i = x0 - (y - y0); i <= x0 + (y - y0); i++)
    			{
    				lcd_point(i, j, pen);
    			}
    		}
		}
		else
		{
			lcd_point(x, y, pen);
    		lcd_point(x, y0 - (y - y0), pen);
    		lcd_point(x0 - (x - x0), y, pen);
    		lcd_point(x0 - (x - x0), y0 - (y - y0), pen);

    		lcd_point(x0 + (y - y0), y0 + (x - x0), pen);
    		lcd_point(x0 + (y - y0), y0 - (x - x0), pen);
    		lcd_point(x0 - (y - y0), y0 + (x - x0), pen);
    		lcd_point(x0 - (y - y0), y0 - (x - x0), pen);
		}
		if (d < 0)
		{
			d = d +  ((x - x0) << 2) + 6;
		}
		else
		{
			d = d + (((x - x0) - (y - y0)) << 2) + 10;
			y--;
		}
		x++;
	} while ((x - x0) <= (y - y0));
	return;
} // end lcd_circle


//******************************************************************************
//	draw square of radius r0 and center x0,y0
//
//	pen =	0000 0000
//	         \\\\ \\\\
//	          \\\\ \\\\_ 0=erase, 1=draw
//	           \\\\ \\\_ 0=single, 1=double
//	            \\\\ \\_ 0=no fill, 1=fill
//
void lcd_square(int16 x0, int16 y0, uint16 r0, uint8 pen)
{
#if 0
	int16  x, y;

	for (x = x0-r0; x <= (x0+r0); x++)
	{
		lcd_point(x, y0-r0, pen);
		lcd_point(x, y0+r0, pen);
	}
	for (y = y0-r0; y <= (y0+r0); y++)
	{
		lcd_point(x0-r0, y, pen);
		lcd_point(x0+r0, y, pen);
	}
#endif
	lcd_rectangle(x0 - r0, y0 - r0, r0 + r0, r0 + r0, pen);
	return;
} // end lcd_square


//******************************************************************************
//	draw rectangle at lower left (x0,y0) of width w, height h
//
//	pen =	0000 0000
//	         \\\\ \\\\
//	          \\\\ \\\\_ 0=erase, 1=draw
//	           \\\\ \\\_ 0=single, 1=double
//	            \\\\ \\_ 0=no fill, 1=fill
//
void lcd_rectangle(int16 x, int16 y, uint16 w, uint16 h, uint8 pen)
{
	int16  x0, y0;
	int8 fill_flag = (pen & 0x04) ? 1 : 0;
	pen &= 0x03;

	if (w-- == 0) return;
	for (y0 = y; y0 <= y + h; y0++)
	{
		lcd_point(x, y0, pen);
		if ((y0 == y) || (y0 == y + h) || fill_flag)
		{
			for (x0 = x + 1; x0 < x + w; x0++)
			{
				lcd_point(x0, y0, pen);
			}
		}
		lcd_point(x + w, y0, pen);
	}
	return;
} // end lcd_rectangle


//******************************************************************************
//******************************************************************************
//	ASCII character set for LCD
//
const unsigned char cs[][5] = {
//	-----  -OO--  OO-OO  -----  -O---  OO--O  -O---  -OO--
//	-----  -OO--  OO-OO  -O-O-  -OOO-  OO--O  O-O--  -OO--
//	-----  -OO--  O--O-  OOOOO  O----  ---O-  O-O--  -----
//	-----  -OO--  -----  -O-O-  -OO--  --O--  -O---  -----
//	-----  -OO--  -----  -O-O-  ---O-  -O---  O-O-O  -----
//	-----  -----  -----  OOOOO  OOO--  O--OO  O--O-  -----
//	-----  -OO--  -----  -O-O-  --O--  O--OO  -OO-O  -----
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0x00,0x00,0x00,0x00,0x00 },		// SP
	{ 0xfa,0xfa,0x00,0x00,0x00 },		// !
	{ 0xe0,0xc0,0x00,0xe0,0xc0 },		// "
	{ 0x24,0x7e,0x24,0x7e,0x24 },		// #
	{ 0x24,0xd4,0x56,0x48,0x00 },		// $
	{ 0xc6,0xc8,0x10,0x26,0xc6 },		// %
	{ 0x6c,0x92,0x6a,0x04,0x0a },		// &
	{ 0xc0,0xc0,0x00,0x00,0x00 },		// '

//	---O-  -O---  -----  -----  -----  -----  -----  -----
//	--O--  --O--  -O-O-  --O--  -----  -----  -----  ----O
//	--O--  --O--  -OOO-  --O--  -----  -----  -----  ---O-
//	--O--  --O--  OOOOO  OOOOO  -----  OOOOO  -----  --O--
//	--O--  --O--  -OOO-  --O--  -----  -----  -----  -O---
//	--O--  --O--  -O-O-  --O--  -OO--  -----  -OO--  O----
//	---O-  -O---  -----  -----  -OO--  -----  -OO--  -----
//	-----  -----  -----  -----  -O---  -----  -----  -----
	{ 0x7c,0x82,0x00,0x00,0x00 },		// (
	{ 0x82,0x7c,0x00,0x00,0x00 },		// )
	{ 0x10,0x7c,0x38,0x7c,0x10 },		// #
	{ 0x10,0x10,0x7c,0x10,0x10 },		// +
	{ 0x07,0x06,0x00,0x00,0x00 },		// ,
	{ 0x10,0x10,0x10,0x10,0x10 },		// -
	{ 0x06,0x06,0x00,0x00,0x00 },		// .
	{ 0x04,0x08,0x10,0x20,0x40 },		// slash

//	-OOO-  --O--  -OOO-  -OOO-  ---O-  OOOOO  --OOO  OOOOO
//	O---O  -OO--  O---O  O---O  --OO-  O----  -O---  ----O
//	O--OO  --O--  ----O  ----O  -O-O-  O----  O----  ---O-
//	O-O-O  --O--  --OO-  -OOO-  O--O-  OOOO-  OOOO-  --O--
//	OO--O  --O--  -O---  ----O  OOOOO  ----O  O---O  -O---
//	O---O  --O--  O----  O---O  ---O-  O---O  O---O  -O---
//	-OOO-  -OOO-  OOOOO  -OOO-  ---O-  -OOO-  -OOO-  -O---
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0x7c,0x8a,0x92,0xa2,0x7c },		// 0
//	{ 0x42,0xfe,0x02,0x00,0x00 },		// 1
	{ 0x00,0x42,0xfe,0x02,0x00 },		// 1
	{ 0x46,0x8a,0x92,0x92,0x62 },		// 2
	{ 0x44,0x92,0x92,0x92,0x6c },		// 3
	{ 0x18,0x28,0x48,0xfe,0x08 },		// 4
	{ 0xf4,0x92,0x92,0x92,0x8c },		// 5
	{ 0x3c,0x52,0x92,0x92,0x8c },		// 6
	{ 0x80,0x8e,0x90,0xa0,0xc0 },		// 7

//	-OOO-  -OOO-  -----  -----  ---O-  -----  -O---  -OOO-
//	O---O  O---O  -----  -----  --O--  -----  --O--  O---O
//	O---O  O---O  -OO--  -OO--  -O---  OOOOO  ---O-  O---O
//	-OOO-  -OOOO  -OO--  -OO--  O----  -----  ----O  --OO-
//	O---O  ----O  -----  -----  -O---  -----  ---O-  --O--
//	O---O  ---O-  -OO--  -OO--  --O--  OOOOO  --O--  -----
//	-OOO-  -OO--  -OO--  -OO--  ---O-  -----  -O---  --O--
//	-----  -----  -----  -O---  -----  -----  -----  -----
	{ 0x6c,0x92,0x92,0x92,0x6c },		// 8
	{ 0x60,0x92,0x92,0x94,0x78 },		// 9
	{ 0x36,0x36,0x00,0x00,0x00 },		// :
	{ 0x37,0x36,0x00,0x00,0x00 },		// ;
	{ 0x10,0x28,0x44,0x82,0x00 },		// <
	{ 0x24,0x24,0x24,0x24,0x24 },		// =
	{ 0x82,0x44,0x28,0x10,0x00 },		// >
	{ 0x60,0x80,0x9a,0x90,0x60 },		// ?

//	-OOO-  -OOO-  OOOO-  -OOO-  OOOO-  OOOOO  OOOOO  -OOO-
//	O---O  O---O  O---O  O---O  O---O  O----  O----  O---O
//	O-OOO  O---O  O---O  O----  O---O  O----  O----  O----
//	O-O-O  OOOOO  OOOO-  O----  O---O  OOOO-  OOOO-  O-OOO
//	O-OOO  O---O  O---O  O----  O---O  O----  O----  O---O
//	O----  O---O  O---O  O---O  O---O  O----  O----  O---O
//	-OOO-  O---O  OOOO-  -OOO-  OOOO   OOOOO  O----  -OOO-
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0x7c,0x82,0xba,0xaa,0x78 },		// @
	{ 0x7e,0x90,0x90,0x90,0x7e },		// A
	{ 0xfe,0x92,0x92,0x92,0x6c },		// B
	{ 0x7c,0x82,0x82,0x82,0x44 },		// C
	{ 0xfe,0x82,0x82,0x82,0x7c },		// D
	{ 0xfe,0x92,0x92,0x92,0x82 },		// E
	{ 0xfe,0x90,0x90,0x90,0x80 },		// F
	{ 0x7c,0x82,0x92,0x92,0x5c },		// G

//	O---O  -OOO-  ----O  O---O  O----  O---O  O---O  -OOO-
//	O---O  --O--  ----O  O--O-  O----  OO-OO  OO--O  O---O
//	O---O  --O--  ----O  O-O--  O----  O-O-O  O-O-O  O---O
//	OOOOO  --O--  ----O  OO---  O----  O---O  O--OO  O---O
//	O---O  --O--  O---O  O-O--  O----  O---O  O---O  O---O
//	O---O  --O--  O---O  O--O-  O----  O---O  O---O  O---O
//	O---O  -OOO-  -OOO-  O---O  OOOOO  O---O  O---O  -OOO-
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0xfe,0x10,0x10,0x10,0xfe },		// H
	{ 0x82,0xfe,0x82,0x00,0x00 },		// I
	{ 0x0c,0x02,0x02,0x02,0xfc },		// J
	{ 0xfe,0x10,0x28,0x44,0x82 },		// K
	{ 0xfe,0x02,0x02,0x02,0x02 },		// L
	{ 0xfe,0x40,0x20,0x40,0xfe },		// M
	{ 0xfe,0x40,0x20,0x10,0xfe },		// N
	{ 0x7c,0x82,0x82,0x82,0x7c },		// O

//	OOOO-  -OOO-  OOOO-  -OOO-  OOOOO  O---O  O---O  O---O
//	O---O  O---O  O---O  O---O  --O--  O---O  O---O  O---O
//	O---O  O---O  O---O  O----  --O--  O---O  O---O  O-O-O
//	OOOO-  O-O-O  OOOO-  -OOO-  --O--  O---O  O---O  O-O-O
//	O----  O--OO  O--O-  ----O  --O--  O---O  O---O  O-O-O
//	O----  O--O-  O---O  O---O  --O--  O---O  -O-O-  O-O-O
//	O----  -OO-O  O---O  -OOO-  --O--  -OOO-  --O--  -O-O-
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0xfe,0x90,0x90,0x90,0x60 },		// P
	{ 0x7c,0x82,0x92,0x8c,0x7a },		// Q
	{ 0xfe,0x90,0x90,0x98,0x66 },		// R
	{ 0x64,0x92,0x92,0x92,0x4c },		// S
	{ 0x80,0x80,0xfe,0x80,0x80 },		// T
	{ 0xfc,0x02,0x02,0x02,0xfc },		// U
	{ 0xf8,0x04,0x02,0x04,0xf8 },		// V
	{ 0xfc,0x02,0x3c,0x02,0xfc },		// W

//	O---O  O---O  OOOOO  -OOO-  -----  -OOO-  --O--  -----
//	O---O  O---O  ----O  -O---  O----  ---O-  -O-O-  -----
//	-O-O-  O---O  ---O-  -O---  -O---  ---O-  O---O  -----
//	--O--  -O-O-  --O--  -O---  --O--  ---O-  -----  -----
//	-O-O-  --O--  -O---  -O---  ---O-  ---O-  -----  -----
//	O---O  --O--  O----  -O---  ----O  ---O-  -----  -----
//	O---O  --O--  OOOOO  -OOO-  -----  -OOO-  -----  OOOOO
//	-----  -----  -----  -----  -----  -----  -----  -----
	{ 0xc6,0x28,0x10,0x28,0xc6 },		// O
	{ 0xe0,0x10,0x0e,0x10,0xe0 },		// Y
	{ 0x86,0x8a,0x92,0xa2,0xc2 },		// Z
	{ 0xfe,0x82,0x82,0x00,0x00 },		// [
	{ 0x40,0x20,0x10,0x08,0x04 },		// back slash
	{ 0x82,0x82,0xfe,0x00,0x00 },		// ]
	{ 0x20,0x40,0x80,0x40,0x20 },		// ^
	{ 0x02,0x02,0x02,0x02,0x02 },		// _

//	-OO--  -----  O----  -----  ----O  -----  --OOO  -----
//	-OO--  -----  O----  -----  ----O  -----  -O---  -----
//	--O--  -OOO-  OOOO-  -OOO-  -OOOO  -OOO-  -O---  -OOOO
//	-----  ----O  O---O  O---O  O---O  O---O  OOOO-  O---O
//	-----  -OOOO  O---O  O----  O---O  OOOO-  -O---  O---O
//	-----  O---O  O---O  O---O  O---O  O----  -O---  -OOOO
//	-----  -OOOO  OOOO-  -OOO-  -OOOO  -OOO-  -O---  ----O
//	-----  -----  -----  -----  -----  -----  -----  -OOO-
	{ 0xc0,0xe0,0x00,0x00,0x00 },		// `
	{ 0x04,0x2a,0x2a,0x2a,0x1e },		// a
	{ 0xfe,0x22,0x22,0x22,0x1c },		// b
	{ 0x1c,0x22,0x22,0x22,0x14 },		// c
	{ 0x1c,0x22,0x22,0x22,0xfc },		// d
	{ 0x1c,0x2a,0x2a,0x2a,0x10 },		// e
	{ 0x10,0x7e,0x90,0x90,0x80 },		// f
	{ 0x18,0x25,0x25,0x25,0x3e },		// g

//	O----  -O---  ----O  O----  O----  -----  -----  -----
//	O----  -----  -----  O----  O----  -----  -----  -----
//	O----  -O---  ---OO  O--O-  O----  OO-O-  OOOO-  -OOO-
//	OOOO-  -O---  ----O  O-O--  O----  O-O-O  O---O  O---O
//	O---O  -O---  ----O  OO---  O----  O-O-O  O---O  O---O
//	O---O  -O---  ----O  O-O--  O----  O---O  O---O  O---O
//	O---O  -OO--  O---O  O--O-  OO---  O---O  O---O  -OOO-
//	-----  -----  -OOO-  -----  -----  -----  -----  -----
	{ 0xfe,0x10,0x10,0x10,0x0e },		// h
	{ 0xbe,0x02,0x00,0x00,0x00 },		// i
	{ 0x02,0x01,0x01,0x21,0xbe },		// j
	{ 0xfe,0x08,0x14,0x22,0x00 },		// k
	{ 0xfe,0x02,0x00,0x00,0x00 },		// l
	{ 0x3e,0x20,0x18,0x20,0x1e },		// m
	{ 0x3e,0x20,0x20,0x20,0x1e },		// n
	{ 0x1c,0x22,0x22,0x22,0x1c },		// o

//	-----  -----  -----  -----  -----  -----  -----  -----
//	-----  -----  -----  -----  -O---  -----  -----  -----
//	OOOO-  -OOOO  O-OO-  -OOO-  OOOO-  O--O-  O---O  O---O
//	O---O  O---O  -O--O  O----  -O---  O--O-  O---O  O---O
//	O---O  O---O  -O---  -OOO-  -O---  O--O-  O---O  O-O-O
//	O---O  O---O  -O---  ----O  -O--O  O-OO-  -O-O-  OOOOO
//	OOOO-  -OOOO  OOO--  OOOO-  --OO-  -O-O-  --O--  -O-O-
//	O----  ----O  -----  -----  -----  -----  -----  -----
	{ 0x3f,0x22,0x22,0x22,0x1c },		// p
	{ 0x1c,0x22,0x22,0x22,0x3f },		// q
	{ 0x22,0x1e,0x22,0x20,0x10 },		// r
	{ 0x12,0x2a,0x2a,0x2a,0x04 },		// s
	{ 0x20,0x7c,0x22,0x22,0x04 },		// t
	{ 0x3c,0x02,0x04,0x3e,0x00 },		// u
	{ 0x38,0x04,0x02,0x04,0x38 },		// v
	{ 0x3c,0x06,0x0c,0x06,0x3c },		// w

//	-----  -----  -----  ---OO  --O--  OO---  -O-O-  -OO--
//	-----  -----  -----  --O--  --O--  --O--  O-O--  O--O-
//	O---O  O--O-  OOOO-  --O--  --O--  --O--  -----  O--O-
//	-O-O-  O--O-  ---O-  -OO--  -----  --OO-  -----  -OO--
//	--O--  O--O-  -OO--  --O--  --O--  --O--  -----  -----
//	-O-O-  -OOO-  O----  --O--  --O--  --O--  -----  -----
//	O---O  --O--  OOOO-  ---OO  --O--  OO---  -----  -----
//	-----  OO---  -----  -----  -----  -----  -----  -----
	{ 0x22,0x14,0x08,0x14,0x22 },		// x
	{ 0x39,0x05,0x06,0x3c,0x00 },		// y
	{ 0x26,0x2a,0x2a,0x32,0x00 },		// z
	{ 0x10,0x7c,0x82,0x82,0x00 },		// {
	{ 0xee,0x00,0x00,0x00,0x00 },		// |
	{ 0x82,0x82,0x7c,0x10,0x00 },		// }
	{ 0x40,0x80,0x40,0x80,0x00 },		// ~
//	{ 0x02,0x06,0x0a,0x06,0x02 }		// _
	{ 0x60,0x90,0x90,0x60,0x00 }		// _
};


//******************************************************************************
//	set lcd cursor position
//
//	Description: set the position at which the next character will be printed.
//
uint8 lcd_cursor(int16 x, int16 y)
{
	lcd_x = ((x >= 0) && (x < HD_X_MAX)) ? x : HD_X_MAX-1;
	lcd_y = ((y >= 0) && (y < HD_Y_MAX)) ? y : HD_Y_MAX-1;
	return 0;
} // end lcd_cursor


//******************************************************************************
//	write data to LCD
//
//	lcd_y = lower left-hand corner
//	lcd_x = column
//
void lcd_WD(uint8 datum)
{
	int i;
	int y = lcd_y + CHAR_SIZE -1;

	if (lcd_dmode & LCD_REVERSE_FONT) datum = ~datum;

	for (i=0x80; i; i >>= 1)
	{
		if (i & datum)
		{
			lcd_point(lcd_x, y, 1);
		}
		else
		{
			if (!(lcd_dmode & LCD_OR_CHAR))
				lcd_point(lcd_x, y, 0);
		}
		y--;
	}
	return;
} // end lcd_WD


//******************************************************************************
//	write character to LCD
//
char lcd_putchar(char c)
{
	int i;

	switch (c)
	{
		case '\a':
		{
			lcd_dmode |= LCD_REVERSE_FONT;
			break;
		}

		case '\n':
		{
			lcd_y = (lcd_y - CHAR_SIZE * (lcd_dmode & ~LCD_2X_FONT ? 2 : 1)) % HD_Y_MAX;
		}

		case '\r':
		{
		 	lcd_x = 0;
		 	break;
		}

		default:
		{
			if ((c >= ' ') && (c <= '~'))
			{
				if (lcd_dmode & LCD_2X_FONT)
				{
					// leading space
					lcd_y = (lcd_y + CHAR_SIZE) % HD_Y_MAX;
 					lcd_WD(0x00);
					lcd_y = (lcd_y - CHAR_SIZE) % HD_Y_MAX;
 					lcd_WD(0x00);
					if (++lcd_x >= HD_X_MAX) lcd_x = 0;

					for (i = 0; i < 5; i++)
					{
						unsigned char mask1 = 0x01;
						unsigned int mask2 = 0x0001;
						unsigned int data = 0;

						while (mask1)
						{
							// double bits into data
							if (cs[c - ' '][i] & mask1) data |= mask2 | (mask2 << 1);
							mask1 <<= 1;
							mask2 <<= 2;
						}
						lcd_y = (lcd_y + CHAR_SIZE) % HD_Y_MAX;
	 					lcd_WD(data >> 8);
						lcd_y = (lcd_y - CHAR_SIZE) % HD_Y_MAX;
			 			lcd_WD(data & 0x00ff);
						if (++lcd_x >= HD_X_MAX) lcd_x = 0;

						lcd_y = (lcd_y + CHAR_SIZE) % HD_Y_MAX;
	 					lcd_WD(data >> 8);
						lcd_y = (lcd_y - CHAR_SIZE) % HD_Y_MAX;
			 			lcd_WD(data & 0x00ff);
						if (++lcd_x >= HD_X_MAX) lcd_x = 0;

						if (i && (lcd_dmode & LCD_PROPORTIONAL) && !cs[c - ' '][i]) break;
					}
					// trailing space
					lcd_y = (lcd_y + CHAR_SIZE) % HD_Y_MAX;
 					lcd_WD(0x00);
					lcd_y = (lcd_y - CHAR_SIZE) % HD_Y_MAX;
 					lcd_WD(0x00);
					if (++lcd_x >= HD_X_MAX) lcd_x = 0;
				}
				else
				{
					lcd_WD(0x00);					// leading space
					for (i = 0; i < 5; )
					{
						lcd_WD(cs[c - ' '][i++]);	// output character
						if (++lcd_x >= HD_X_MAX) lcd_x = 0;

						// check proportional flag
						if (i && (lcd_dmode & LCD_PROPORTIONAL) && !cs[c - ' '][i]) break;
					}
					lcd_WD(0x00);					// trailing space
					if (++lcd_x >= HD_X_MAX) lcd_x = 0;
				}
			}
		}
	}
	return c;
} // end my_putchar


//******************************************************************************
//	formatted print to lcd
//
uint16 lcd_printf(const char* fmt, ...)
{
	char printBuffer[PRINT_BUFFER_SIZE+1];
	char* s_ptr = printBuffer;
	uint16 s_length = 0;
	va_list arg_ptr;

	if (strlen(fmt) > PRINT_BUFFER_SIZE) ERROR2(SYS_ERR_PRINT);

	va_start(arg_ptr, fmt);					// create pointer to args
	vsprintf(s_ptr, fmt, arg_ptr);			// generate print string
	while (*s_ptr)
	{
		lcd_putchar(*s_ptr++);				// output string
		s_length++;
	}
	va_end(arg_ptr);						// destroy arg pointer
	return s_length;
} // end lcd_printf
