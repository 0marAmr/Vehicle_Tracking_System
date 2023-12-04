/******************************************************************************
 *
 * Module: Internal EEPROM
 *
 * File Name: Internal_EEPROM.c
 *
 * Description: Source file for the AVR EEPROM driver
 *
 * Author: Omar Muhammad
 *
 *******************************************************************************/
#include "Internal_EEPROM.h"
#include "common_macros.h"	/* To use the macros like SET_BIT */
#include "avr/io.h"			/* To use the Internal EEPROM Registers */


#include "GPIO.h"
#include "std_types.h"

/*
 * Description :
 * Write byte in the internal EEPROM of ATMEGA32
 */
void EEPROMINTENAL_writeByte (const uint16 address, const uint8 data)
{
	/* polling on the EEWE until it becomes zero
	 * EECR: The EEPROM Control Register
	 * EEWE: EEPROM Write Enable
	 * */
	while (BIT_IS_SET(EECR,EEWE));

	/* Setting the address in the EEAR: EEPROM Address Register */
	EEAR = address;
	/* Setting the data in the EEDR: EEPROM Data Register */
	EEDR = data;

	/* start EEPROM write by setting EEWE. */
	SET_BIT(EECR,EEMWE); /* Write logical one to EEPROM Master Write Enable */

	/* Start Writing */
	asm("SBI 0x1C,1"); /*Equivalent in C: SET_BIT(EECR,EEWE) */
}

/*
 * Description :
 * Read byte in the internal EEPROM of ATMEGA32
 */
uint8 EEPROMINTENAL_readByte (const uint16 address)
{
	/* polling on the EEWE bit before starting the read operation.
	 * If a write operation is in progress, it is neither possible to read the EEPROM,
	 * nor to change the EEAR Register.
	 *
	 * EECR: The EEPROM Control Register
	 * EEWE: EEPROM Write Enable
	 * */
	while (BIT_IS_SET(EECR,EEWE));

	/* Setting the address in the EEAR: EEPROM Address Register */
	EEAR = address;

	/* Start reading operation by setting EERE : EEPROM Read Enable */
	SET_BIT(EECR,EERE);

	/* Return the read Data from EEPROM by returning the EEPROM Data Register (EEDR) */
	return EEDR;
}
