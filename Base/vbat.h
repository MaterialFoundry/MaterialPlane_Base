/*
 * vbat.h
 *
 * Created: 05/12/2022 14:59:06
 *  Author: Cris
 */ 

#ifndef VBAT_H_
#define VBAT_H_

uint16_t vBatOld = 0;						//Store old battery voltage

/*
	Configure battery voltage measurement system
 */
void configureVbat() {
	//Configure Vref
	VREF.CTRLB |= VREF_ADC1REFEN_bm;		// Enable ADC1 Vref
	VREF.CTRLC = VREF_ADC1REFSEL_2V5_gc;	// Set ADC1 Vref to 2.5V
	
	//Configure ADC1
	ADC1.MUXPOS = ADC_MUXPOS_INTREF_gc;		// Set input to Vref
	ADC1.CTRLC = ADC_PRESC_DIV256_gc		// Set clock to SYSCLK/256
		| ADC_REFSEL_VDDREF_gc				// Set Vcc as reference
		| 0 << ADC_SAMPCAP_bp;				// Disable sample capacitance
	ADC1.CTRLA = 1 << ADC_ENABLE_bp			// Enable ADC
		| 0 << ADC_FREERUN_bp				// Enable freerun mode
		| ADC_RESSEL_10BIT_gc;				// Set to 10-bit mode
	ADC1.CTRLB = ADC_SAMPNUM_ACC64_gc;		// Set oversampling to 64
	ADC1.COMMAND |= 1;						// start running ADC
}

uint16_t getBatteryVoltage() {
	if (ADC1.INTFLAGS) {
		ADC1.COMMAND |= 1;
		vBatOld = (uint16_t)(-0.077*ADC1.RES + 7200);
		return vBatOld;
	}
	else {
		return vBatOld;
	}
}

uint8_t getBatteryState() {
	uint16_t voltage = getBatteryVoltage();
	if (voltage >= 3800) return 6;	//85-100%
	if (voltage >= 3650) return 5;	//70-85%
	if (voltage >= 3500) return 4;	//55-70%
	if (voltage >= 3450) return 3;	//40-55%
	if (voltage >= 3300) return 2;	//25-40%
	if (voltage >= 3000) return 1;	//10-25%
	return 0;						//0-10%
}

#endif /* VBAT_H_ */