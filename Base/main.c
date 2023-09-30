#include <atmel_start.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "definitions.h"
#include "sleep.h"
#include "vbat.h"
#include "timer.h"
#include "eeprom.h"
#include "irLed.h"
#include "redLed.h"

//#define INVERTED_MODE
#define INVERTED_MODE_SENS	75

uint8_t const FW_VERSION[3] = {2,0,1};
extern volatile uint8_t measurement_done_touch;
uint8_t stopSent = false;
uint16_t serialNumber;
uint16_t vBat = 0;
uint8_t vBatState = 0;
uint8_t leftTouchState, rightTouchState = false;	//Stores touch states for both touch pads
uint8_t touchSensitivity = 80;
uint64_t timeSinceLastTouch = 0;
uint8_t currentlyTouched = 0;
uint8_t touchCounter = 0;
uint64_t lastTouchTime = 0;
uint8_t mode = MODE_NORMAL;

int main(void)
{
	//Initialize MCU, drivers and middleware 
	atmel_start_init();
	
	#ifdef SERIAL_DEBUG
		printf("\n\n----------------------------------\nInitializing\n\n");
		printf("Firmware Version:\tv%d.", FW_VERSION[0]);
		printf("%d.", FW_VERSION[1]);
		printf("%d\n", FW_VERSION[2]);
	#endif
	
	//clearEeprom();
	
	initializeRedLed();
	
	eepromPutDevice(DEVICE_ID);
	
	eepromPutVersion(FW_VERSION[0], FW_VERSION[1], FW_VERSION[2]);
	
	serialNumber = SIGROW.SERNUM8<<8 | SIGROW.SERNUM9;	//Get the uC's serial number
	#ifdef SERIAL_DEBUG
		printf("Serial Number:\t\t%d\n", serialNumber);
	#endif
	if (eepromGetId() == 0xFFFF) eepromPutId(serialNumber);
	else serialNumber = eepromGetId();
	#ifdef SERIAL_DEBUG
		printf("Device ID:\t\t%d\n", serialNumber);
	#endif
	
	if (eepromGetSensitivity() == 0xFF) eepromPutSensitivity(touchSensitivity);
	else touchSensitivity = eepromGetSensitivity();
	
	//Configure pins for testing
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	PORTB.PIN3CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
	
	TCA0.SINGLE.INTCTRL = 1;							//Enable interrupt for TCA0 timer
	PORTA_PIN4CTRL |= 0x01;								//Set interrupt for enable switch
	configureSleep();									//Configure sleep
	configureVbat();									//Configure battery monitor
	vBatState = getBatteryState();						//Get battery state
	sei();												//Enable interrupts
	
	if ((PORTB.IN >> 2) & 1 || (PORTB.IN >> 3) & 1) {
		while(1) {
			if ((PORTB.IN >> 2) & 1 || (PORTB.IN >> 3) & 1) {
				setIrLed(LED_OFF);
				setRedLed(true);
			}
			else {
				setRedLed(false);
				setIrLed(LED_HIGH);
				
			}
		}
	}
	
	blink(100, 4);										//Blink 4 times to indicate wake-up
	
	configureIrLed();
	
	//touch_sensors_config();
	touch_init(100-touchSensitivity);
	//touch_init(100-80);
	
	#ifdef INVERTED_MODE
		touch_init(100-(uint8_t)INVERTED_MODE_SENS);
	#endif
	
	pinsToLowPowerMode();
	
	#ifdef SERIAL_DEBUG
		printf("Battery Voltage:\t%dmV\n", getBatteryVoltage());
		printf("Battery State:\t\t%d\n", getBatteryState());
		printf("Touch Sensitivity:\t%d\n", touchSensitivity);
		printf("Touch Threshold:\t%d\n", 100-touchSensitivity);
	#endif
	
	//clearEeprom();
	#ifdef SERIAL_DEBUG
		printf("\nDone initializing\n----------------------------------\n\n");
	#endif
	
	while (1) {
		//If switch is high, get ready to sleep
		if (enSw_get_level()) {
			#ifdef SERIAL_DEBUG
			printf("Sleeping\n");
			#endif
			setIrLed(LED_OFF);	//Switch ir led off
			setRedLed(false);	//Switch red led off
			blink(100, 2);		//Blink twice to indicate sleep
			sleep();			//Go to sleep
		}
			
		//Check battery voltage every 10 seconds
		if (timer1Elapsed() >= 500) {
			vBatState = getBatteryState();	//Get battery state
			//serialNumber = getBatteryVoltage();
			timer1Reset();					//Reset timer
		}
			
		//Check for touches
		touch_process();
		if (measurement_done_touch) {
			measurement_done_touch = false;
			leftTouchState = get_sensor_state(LEFT_TOUCHPAD) >> 7;
			rightTouchState = get_sensor_state(RIGHT_TOUCHPAD) >> 7;
			
			#ifdef INVERTED_MODE
				if (!rightTouchState) {
					
					setRedLed(true);								//Switch red led on
					sendIr(CMD, serialNumber, vBatState);			//Send ir command
					stopSent = 0;									//Switch stopSent to false so the stop command can be sent again when base is no longer touched
				}
				else {
					//resetCalibrationTimer();						//Reset calibration timer
					//If stop command has not been sent yet
					if (stopSent == 0) {
						stopSent = 1;								//Set stopSent to true to prevent duplicate sends
						sendIr(CMD, serialNumber, vBatState);		//Send ir command
						sendIr(CMD, serialNumber, vBatState);		//Send ir command
					}
					else if (stopSent == 1) {
						stopSent = 2;
						sendIr(CMD_STOP, serialNumber, vBatState);	//Send stop command
						setIrLed(LED_OFF);							//Switch ir led off
						setRedLed(false);							//Switch red led off
					}
				}
			#else
				#ifdef TOUCH_DEBUG
				uint16_t sigL = get_sensor_node_signal(LEFT_TOUCHPAD);
				uint16_t refL = get_sensor_node_reference(LEFT_TOUCHPAD);
				uint16_t sigR = get_sensor_node_signal(RIGHT_TOUCHPAD);
				uint16_t refR = get_sensor_node_reference(RIGHT_TOUCHPAD);
				uint16_t threshold = 100-touchSensitivity;
				printf("SigL: %d\t", sigL);
				printf("RefL: %d\t", refL);
				printf("DelL:  %d", (int16_t)(sigL-refL));
				printf("/%d\t\t", threshold);
				printf("SigR: %d\t", sigR);
				printf("RefR: %d\t", refR);
				printf("DelL:  %d", (int16_t)(sigR-refR));
				printf("/%d\n", threshold);
				#endif
				
				//Detect touch length and period to determine mode switch
				if (!leftTouchState) {
					if (rightTouchState && !currentlyTouched) {
						if (mode == MODE_ALWAYS_ON) setRedLed(false);
						currentlyTouched = 1;
						lastTouchTime = lastTouchTimerElepsed();
						#ifdef SERIAL_DEBUG
						printf("Time elapsed since last touch: %d\n", lastTouchTime);
						#endif
						if (lastTouchTime > 1500) touchCounter = 0;
						resetTouchTimer();
						resetLastTouchTimer();
					}
					if (mode == MODE_ALWAYS_ON && rightTouchState && currentlyTouched && touchTimerElepsed() >= TOUCH_TIME_ALWAYS_ON_LED) {
						#ifdef SERIAL_DEBUG
						printf("Switch red led on\n");
						#endif
						setRedLed(true);
					}
					if (!rightTouchState && currentlyTouched) {
						currentlyTouched = 0;
						if (touchTimerElepsed() < TOUCH_TIME_MAX_LENGTH && lastTouchTime < TOUCH_TIME_MAX_PERIOD) touchCounter++;
						#ifdef SERIAL_DEBUG
						printf("Touch time: %d\t", touchTimerElepsed());
						printf("LastTouchTime: %d\t", lastTouchTime);
						printf("TouchCounter: %d\n", touchCounter);
						#endif
						//If touch pad has been touched enough times
						if (touchCounter >= TOUCH_COUNT) {
							//Switch mode
							if (mode == MODE_NORMAL) mode = MODE_ALWAYS_ON;
							else mode = MODE_NORMAL;
							#ifdef SERIAL_DEBUG
							printf("Change mode: %d\n", mode);
							#endif
							blink(50, 10);										//Blink to indicate current mode
							_delay_ms(500);
							blink(1000, mode+1);
							_delay_ms(500);
						}
						if (mode == MODE_ALWAYS_ON) setRedLed(true);			//Switch red led on
					}
				}
				
				//Recalibrate every second if touch pads are not touched
				if (!leftTouchState && !rightTouchState && calibrationTimerElapsed() >= 1000) {
					resetCalibrationTimer();							//Reset calibration timer
					calibrate_node(RIGHT_TOUCHPAD);						//Perform calibration
					calibrate_node(LEFT_TOUCHPAD);						//Perform calibration
				}
				
				
				if (mode == MODE_NORMAL) {
					//If either touchpad is touched
					if (leftTouchState || rightTouchState) {
						resetCalibrationTimer();						//Reset calibration timer
						setRedLed(true);								//Switch red led on
						sendIr(CMD, serialNumber, vBatState);			//Send ir command
						stopSent = 0;									//Switch stopSent to false so the stop command can be sent again when base is no longer touched
					}
					else {
						//If stop command has not been sent yet
						if (stopSent == 0) {
							stopSent = 1;								//Set stopSent to true to prevent duplicate sends
							sendIr(CMD, serialNumber, vBatState);		//Send ir command
							sendIr(CMD, serialNumber, vBatState);		//Send ir command
						}
						else if (stopSent == 1) {
							stopSent = 2;
							sendIr(CMD_STOP, serialNumber, vBatState);	//Send stop command
							setIrLed(LED_OFF);							//Switch ir led off
							setRedLed(false);							//Switch red led off
						}
					}
				}
				else if (mode == MODE_ALWAYS_ON) {
					sendIr(CMD, serialNumber, vBatState);				//Send ir command
				}
			#endif	
		}
	}
}

/*
 * Set all unused pins to power conserving mode by disabling the input buffer and pullup resistor
 */
 void pinsToLowPowerMode() {
  PORTA.PIN0CTRL = 0x4;     //UPDI
  PORTA.PIN1CTRL = 0x4;    
  PORTA.PIN2CTRL = 0x4;     
  PORTA.PIN3CTRL = 0x4;   
  //PORTA.PIN4CTRL = 0x4;	//EN SW  
  PORTA.PIN5CTRL = 0x4;
  PORTA.PIN6CTRL = 0x4;
  PORTA.PIN7CTRL = 0x4;

  //PORTB.PIN0CTRL = 0x4;	//RED LED
  PORTB.PIN1CTRL = 0x4;
  PORTB.PIN2CTRL = 0x4;		//TX  
  PORTB.PIN3CTRL = 0x4;		//RX
  //PORTB.PIN4CTRL = 0x4;	//IR LED
  PORTB.PIN5CTRL = 0x4;   

  //PORTC.PIN0CTRL = 0x4;	//Touch1_X
  //PORTC.PIN1CTRL = 0x4;	//Touch1_Y
  //PORTC.PIN2CTRL = 0x4;   //Touch2_X
  //PORTC.PIN3CTRL = 0x4;	//Touch2_Y
 }


/************************************************************************/
/* How to fix touch files                                               */
/************************************************************************/
/*
in 'atmel_start.c'
	-comment out 'touch_init();'

in 'qtouch/include/touch_api_ptc.h'
	-add parameter 'uint8_t sensitivity' to 'touch_init()': void touch_init(uint8_t sensitivity);

in 'qtouch/touch.c':
	-in function 'touch_init()'
		-add parameter 'uint8_t sensitivity'
		-add sensitivity parameter to touch_sensors_config(): touch_sensors_config(sensitivity);
	-in function 'touch_sensors_config()' 
		-add parameter 'uint8_t sensitivity' in both prototype (l.60) and actual function (l.176)
		-add the following lines at the start of the function:
			qtlib_key_configs_set1[0] = (qtm_touch_key_config_t){sensitivity, HYST_6_25, NO_AKS_GROUP};
			qtlib_key_configs_set1[1] = (qtm_touch_key_config_t){sensitivity, HYST_6_25, NO_AKS_GROUP};
*/