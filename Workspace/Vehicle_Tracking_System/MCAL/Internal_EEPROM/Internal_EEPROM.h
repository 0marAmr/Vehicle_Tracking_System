 /******************************************************************************
 *
 * Module: Internal EEPROM
 *
 * File Name: Internal_EEPROM.h
 *
 * Description: header file for the AVR EEPROM driver
 *
 * Author: Omar Muhammad
 *
 *******************************************************************************/



#ifndef MCAL_INTERNAL_EEPROM_H_
#define MCAL_INTERNAL_EEPROM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * Write byte in the internal EEPROM of ATMEGA32
 */
void EEPROMINTENAL_writeByte (const uint16 address, const uint8 data);

/*
 * Description :
 * Read byte in the internal EEPROM of ATMEGA32
 */
uint8 EEPROMINTENAL_readByte (const uint16 address);

#endif /* MCAL_INTERNAL_EEPROM_H_ */
