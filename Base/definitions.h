/*
 * definitions.h
 *
 * Created: 16/05/2023 05:10:21
 *  Author: Cris
 */ 


#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define DEVICE_ID	1

//#define SERIAL_DEBUG

/*
 * CMD
 * Sets the command to be sent
 */
#define CMD			1
#define CMD_STOP	0b11100001
//#define CMD_CAL		3
//#define CMD_PIT		5

//@5MHz
#define IR_LOW  2440
#define IR_MID	4875
#define IR_HIGH 7311
#define IR_SHORT	500
#define IR_LONG		1000

//@3.33MHz
//#define IR_LOW  1667
//#define IR_MID	IR_LOW*2
//#define IR_HIGH IR_LOW*3

#define LED_OFF 0
#define LED_HIGH 1
#define LED_PULSE 2

#define TOUCH_TIME_MAX_LENGTH	1000
#define TOUCH_TIME_MAX_PERIOD	2000
#define TOUCH_TIME_ALWAYS_ON_LED	500
#define TOUCH_TIME_CALIBRATION	2000
#define TOUCH_COUNT				4

#define LEFT_TOUCHPAD	0
#define RIGHT_TOUCHPAD	1

//Modes
#define MODE_NORMAL		0
#define MODE_ALWAYS_ON	1

#endif /* DEFINITIONS_H_ */