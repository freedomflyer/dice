//******************************************************************************
//  Lab 8a - Dice
//
//  Description:
//
//	"Write a dice roller C program that waits for a switch to be pressed and then
//	 displays two dice which randomly change values. The dice roll begins rapidly
//	 and progressively slows down until it stops (after approximately 3-5
//	 seconds). A congratulatory ditty is played when doubles are rolled. If
//	 doubles are rolled twice in a row, output a raspberry tone. Write an
//	 assembly language function which returns a random number from 1 to 6 and
//	 call this function from your C program." 
//
//  Author:		Paul Roper, Brigham Young University
//	Revisions:	March 2010	Original code
//				07/28/2011	Added doTone
//
//  Built with Code Composer Studio Version: 4.2.3.00004
//*******************************************************************************
//
//                            MSP430F2274
//                  .-----------------------------.
//            SW1-->|P1.0^                    P2.0|<->LCD_DB0
//            SW2-->|P1.1^                    P2.1|<->LCD_DB1
//            SW3-->|P1.2^                    P2.2|<->LCD_DB2
//            SW4-->|P1.3^                    P2.3|<->LCD_DB3
//       ADXL_INT-->|P1.4                     P2.4|<->LCD_DB4
//        AUX INT-->|P1.5                     P2.5|<->LCD_DB5
//        SERVO_1<--|P1.6 (TA1)               P2.6|<->LCD_DB6
//        SERVO_2<--|P1.7 (TA2)               P2.7|<->LCD_DB7
//                  |                             |
//         LCD_A0<--|P3.0                     P4.0|-->LED_1 (Green)
//        i2c_SDA<->|P3.1 (UCB0SDA)     (TB1) P4.1|-->LED_2 (Orange) / SERVO_3
//        i2c_SCL<--|P3.2 (UCB0SCL)     (TB2) P4.2|-->LED_3 (Yellow) / SERVO_4
//         LCD_RW<--|P3.3                     P4.3|-->LED_4 (Red)
//   TX/LED_5 (G)<--|P3.4 (UCA0TXD)     (TB1) P4.4|-->LCD_BL
//             RX-->|P3.5 (UCA0RXD)     (TB2) P4.5|-->SPEAKER
//           RPOT-->|P3.6 (A6)          (A15) P4.6|-->LED 6 (R)
//           LPOT-->|P3.7 (A7)                P4.7|-->LCD_E
//                  '-----------------------------'
//
//******************************************************************************
//******************************************************************************
// includes
#include "msp430x22x4.h"
#include "dice.h"
#include <stdlib.h>
#include "RBX430-1.h"
#include "RBX430_lcd.h"

//-----------------------------------------------------------
//	external/internal prototypes
extern int rand16(void);				// get random #
extern int rand6(void);				// get random #

void WDT_sleep(uint16 delay);				// WDT sleep routine
uint8 get_switch(uint8 swMask);
void doTone(uint16 tone, uint16 time);		// output tone
void drawDie(uint8 die, uint8* old_die, int16 x, int16 y);

extern const uint8 byu_image[];

//-----------------------------------------------------------
//	global variables
volatile int WDT_Sec_Cnt;				// WDT second counter
volatile int WDT_Delay;					// WDT delay counter
volatile int WDT_Tone_Cnt;				// WDT tone counter

//-----------------------------------------------------------
//	main
void main(void)
{
	uint8 myDie, oldDie = 0;

	RBX430_init(_8MHZ);					// init board
	lcd_init();							// init LCD

	// configure Watchdog
	WDTCTL = WDT_CTL;					// Set Watchdog interval
	WDT_Sec_Cnt = WDT_1SEC_CNT;			// set WD 1 second counter
	WDT_Delay = 0;						// reset delay counter
	WDT_Tone_Cnt = 0;					// turn off tone
	IE1 |= WDTIE;						// enable WDT interrupt

	// configure h/w PWM for speaker
	P4SEL |= 0x20;						// P4.5 TB2 output
	TBR = 0;							// reset timer B
	TBCTL = TBSSEL_2 | ID_0 | MC_1;		// SMCLK, /1, UP (no interrupts)
	TBCCTL2 = OUTMOD_3;					// TB2 = set/reset

	__bis_SR_register(GIE);				// enable interrupts

	lcd_clear();						// clear LCD
	lcd_image(byu_image, (160-91)/2, 120);
	lcd_rectangle(50, 51, 60, 60, 1);	// draw double die frame
	lcd_rectangle(51, 52, 58, 58, 1);


	lcd_mode(LCD_2X_FONT | LCD_PROPORTIONAL);

	while (1)							// repeat forever
	{
		myDie = (rand16() % 6) + 1;		// get a random die (1-6)

		rand6();
		rand6();
		rand6();
		rand6();

		uint8 switchNum = 0;
		switchNum = get_switch(0x0f);
		lcd_clear();
		lcd_cursor(10, 20);				// position message
		lcd_printf("Press Switch");



		uint8 switchNum2 = 0;


		lcd_cursor(10, 20);				// position message
		lcd_printf("Hello World %d ", myDie);
		lcd_backlight(ON);				// turn on LCD
		doTone(BEEP, BEEP_CNT);			// output BEEP
		drawDie(myDie, &oldDie, 57, 57);// draw a die
		WDT_sleep(80);					// delay
		lcd_backlight(OFF);				// turn off LCD
		WDT_sleep(2);					// short delay
	}
} // end main()


uint8 get_switch(uint8 swMask)
{
   uint8 mySwitch;
   uint16 delay = DB_DELAY;

   while ((P1IN & 0x0f) != 0x0f);    // wait for all switches off
   while (delay--)
   {
	  // reset count if switch not pressed
	  mySwitch = (P1IN & 0x0f) ^ 0x0f;
	  if ((mySwitch & swMask) == 0) delay = DB_DELAY;
   };
   return mySwitch;
}

//------------------------------------------------------------------------------
//   output tone subroutine
void doTone(uint16 tone, uint16 time)
{
	TBCCR0 = tone;						// set beep frequency/duty cycle
	TBCCR2 = tone >> 1;					// 50% duty cycle
	WDT_Tone_Cnt = time;				// turn on speaker
	while (WDT_Tone_Cnt);				// wait for tone off
	return;
} // end doTone


//------------------------------------------------------------------------------
//   Watchdog sleep subroutine
void WDT_sleep(uint16 sleep)
{
	if (sleep <= 0) return;
	WDT_Delay = sleep;					// set WD decrementer
	while (WDT_Delay);					// wait for time to expire
	return;
} // end WDT_sleep()


//******************************************************************************
//	draw die
//
//  NOTE: x position needs to be divisible by 3
//
//  dot bit positions:  01    02
//                      04 08 10
//                      20    40
//
const uint8 dice[] = {0x08, 0x41, 0x49, 0x63, 0x6b, 0x77};

const uint8 spot[] = { 15, 15,
//	      0 1 2         3 4 5         6 7 8         9 0 1         2 3 4
	M2B3P(0,0,0), M2B3P(0,0,1), M2B3P(1,1,1), M2B3P(1,0,0), M2B3P(0,0,0),		//	0	--- --O OOO O-- ---
	M2B3P(0,0,0), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(0,0,0),		//	1	--- OOO OOO OOO ---
	M2B3P(0,0,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,0,0),		//	2	--O OOO OOO OOO O--
	M2B3P(0,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,0),		//	3	-OO OOO OOO OOO OO-
	M2B3P(0,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,0),		//	4	-OO OOO OOO OOO OO-
	M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1),		//	5	OOO OOO OOO OOO OOO
	M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1),		//	6	OOO OOO OOO OOO OOO
	M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1),		//	7	OOO OOO OOO OOO OOO
	M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1),		//	8	OOO OOO OOO OOO OOO
	M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1),		//	9	OOO OOO OOO OOO OOO
	M2B3P(0,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,0),		//	10	-OO OOO OOO OOO OO-
	M2B3P(0,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,0),		//	11	-OO OOO OOO OOO OO-
	M2B3P(0,0,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,0,0),		//	12	--O OOO OOO OOO O--
	M2B3P(0,0,0), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(1,1,1), M2B3P(0,0,0),		//	13	--- OOO OOO OOO ---
	M2B3P(0,0,0), M2B3P(0,0,1), M2B3P(1,1,1), M2B3P(1,0,0), M2B3P(0,0,0)		//	14	--- --O OOO O-- ---
};

void drawDie(uint8 die, uint8* old_die, int16 x, int16 y)
{
	uint8 new_die = die = dice[die-1];
	uint8 change = *old_die ^ new_die;		// 1 = change
	int flag = 1;

	// draw any new dots
	new_die &= change;						// 1 = turn on
	do
	{
		if (new_die & 0x01) lcd_image2(spot, x, y+32, flag);
		if (new_die & 0x02) lcd_image2(spot, x+30, y+32, flag);
		if (new_die & 0x04) lcd_image2(spot, x, y+16, flag);
		if (new_die & 0x08) lcd_image2(spot, x+15, y+16, flag);
		if (new_die & 0x10) lcd_image2(spot, x+30, y+16, flag);
		if( new_die & 0x20) lcd_image2(spot, x, y, flag);
		if (new_die & 0x40) lcd_image2(spot, x+30, y, flag);

		// blank any invalid dots
		new_die = *old_die & change;		// 1 = blank
	} while (flag--);

	//	update old die
	*old_die = die;
	return;
} // end outDie()


//------------------------------------------------------------------------------
//	Watchdog Timer ISR
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
	// decrement delay (if non-zero)
	if (WDT_Delay && (--WDT_Delay == 0));

	// decrement tone counter - turn off tone when 0
	if (WDT_Tone_Cnt && (--WDT_Tone_Cnt == 0))
	{
		TBCCR0 = 0;
	}

	if (--WDT_Sec_Cnt == 0)
	{
		WDT_Sec_Cnt = WDT_1SEC_CNT;		// reset counter
		LED_GREEN_TOGGLE;				// toggle green LED
	}
} // end WDT_ISR(void)
