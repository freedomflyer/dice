#ifndef DICE_H_
#define DICE_H_

//------------------------------------------------------------------------------
// defined constants
#define myCLOCK			8000000			// 1.2 Mhz clock
#define	WDT_CTL			WDT_MDLY_32		// WD configuration (Timer, SMCLK, ~32 ms)
#define WDT_CPI			32000			// WDT Clocks Per Interrupt (@1 Mhz)
#define	WDT_1SEC_CNT	myCLOCK/WDT_CPI	// WDT counts/second (32 ms)

#define BEEP			1000			// beep frequency
#define BEEP_CNT		5				// beep duration

// switch debounce function -----------------------------------------
#define SMCLK     8000000
#define DB_TIME   10                 // 10 ms debounce time
#define DB_CYCS   13                 // instruction cycles for delay
#define DB_DELAY  SMCLK*DB_TIME/DB_CYCS/1000 // delay count


#endif /*DICE_H_*/
