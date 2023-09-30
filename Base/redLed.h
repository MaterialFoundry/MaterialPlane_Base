/*
 * redLed.h
 *
 * Created: 16/05/2023 05:15:08
 *  Author: Cris
 */ 


#ifndef REDLED_H_
#define REDLED_H_

void blink(uint8_t delay, uint8_t nrOfBlinks) {
	for (int i=0; i<nrOfBlinks; i++) {
		setRedLed(true);
		for (int i=0; i<delay; i++) _delay_ms(1);
		setRedLed(false);
		for (int i=0; i<delay; i++) _delay_ms(1);
	}
}

void setRedLed(uint8_t val) {
	if (val) PORTB_OUTSET = 0b1;						//Set red led output to 1
	else PORTB_OUTCLR = 0b1;							//Set red led output to 0
}

void initializeRedLed() {
	//Initialize red led
	PORTB_DIRSET = 1;									//Set as output
	PORTB_PIN0CTRL = PORT_INVEN_bm;						//Invert output

	setIrLed(LED_OFF);									//Switch ir led off
}

#endif /* REDLED_H_ */