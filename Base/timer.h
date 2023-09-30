/*
 * timer.h
 *
 * Created: 05/12/2022 16:57:57
 *  Author: Cris
 */ 


#ifndef TIMER_H_
#define TIMER_H_

uint64_t millisTimer = 0;
uint64_t timer1 = 0;
uint64_t touchTimer = 0;
uint64_t lastTouchTimer = 0;
uint64_t calibrationTimer = 0;

uint64_t millis() {
	return millisTimer;
}

uint64_t timer1Elapsed() {
	return millis() - timer1;
}

void timer1Reset() {
	timer1 = millis();
}

uint64_t touchTimerElepsed() {
	return millis() - touchTimer;
}

void resetTouchTimer() {
	touchTimer = millis();
}

uint64_t lastTouchTimerElepsed() {
	return millis() - lastTouchTimer;
}

void resetLastTouchTimer() {
	lastTouchTimer = millis();
}

uint64_t calibrationTimerElapsed() {
	return millis() - calibrationTimer;
}

void resetCalibrationTimer() {
	calibrationTimer = millis();
}

ISR(TCB1_INT_vect)
{
	millisTimer++;
	TCB1.INTFLAGS = TCB_CAPT_bm;
}

#endif /* TIMER_H_ */