/*
 * eeprom.c
 *
 * Created: 14/05/2023 20:27:35
 *  Author: Cris
 */ 
#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/eeprom.h>

#define DEV_ADDR	0x00	//1 byte
#define VER_ADDR	0x01	//3 bytes
#define ID_ADDR		0x04	//2 bytes
#define SENS_ADDR	0x06	//1 byte
#define SENS_ADDR_L	0x07	//1 byte
#define SLEEP_ADDR	0x08	//1 byte


void clearEeprom() {
	for (int i=0; i<50; i++) eeprom_write_byte(i,0xFF);
}

void eepromPutDevice(uint8_t device) {
	if (eeprom_read_byte(DEV_ADDR) != device) eeprom_write_byte(DEV_ADDR, device);
}

void eepromPutVersion(uint8_t v1, uint8_t v2, uint8_t v3) {
	if (eeprom_read_byte(VER_ADDR) != v1) eeprom_write_byte(VER_ADDR, v1);
	if (eeprom_read_byte(VER_ADDR+1) != v2) eeprom_write_byte(VER_ADDR+1, v2);
	if (eeprom_read_byte(VER_ADDR+2) != v3) eeprom_write_byte(VER_ADDR+2, v3);
}

void eepromPutId(uint16_t id) {
	eeprom_write_byte(ID_ADDR, (id>>8) & 0xFF);
	eeprom_write_byte(ID_ADDR+1, id & 0xFF);
}

uint16_t eepromGetId() {
	return eeprom_read_byte(ID_ADDR) << 8 | eeprom_read_byte(ID_ADDR+1);
}

void eepromPutSensitivity(uint8_t sensitivity) {
	eeprom_write_byte(SENS_ADDR, sensitivity);
}

uint8_t eepromGetSensitivity() {
	return eeprom_read_byte(SENS_ADDR);
}

#endif /* EEPROM_H_ */