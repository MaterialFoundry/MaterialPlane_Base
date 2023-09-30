/*
 * sleep.h
 *
 * Created: 21/11/2022 11:47:22
 *  Author: Cris
 */ 


#ifndef SLEEP_H_
#define SLEEP_H_

#include <avr/sleep.h>
#include <avr/interrupt.h>

void configureSleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);		//Set sleep mode
	//set_sleep_mode(SLEEP_MODE_STANDBY);
	sleep_enable();								//Enable sleep
}

void sleep()
{
	RTC.PITINTCTRL = 0;							//Disable PIT interrupts
	uint8_t intOld = RTC.INTCTRL;
	RTC.INTCTRL = 0;
	sleep_cpu();								//Sleep the CPU
	//RTC.INTCTRL = intOld;
	//RTC.PITINTCTRL = RTC_PI_bm;					//Enable PIT interrupts
	
	CCP = CCP_IOREG_gc;							//Enable protected write
	RSTCTRL.SWRR = RSTCTRL_SWRE_bm;				//Restart uController
}

/**
 * PIT ISR
 */
ISR(RTC_PIT_vect) {
	RTC.PITINTFLAGS=RTC_PI_bm;  //clear PIT interrupt flags
}

/*
 * Interrupt service routines: clear interrupt flags
 */
ISR(PORTA_PORT_vect) {
  RTC.PITCTRLA = 0;
  PORTA.INTFLAGS=PORTA.INTFLAGS;  //clear pin interrupt flags
}
#endif /* SLEEP_H_ */