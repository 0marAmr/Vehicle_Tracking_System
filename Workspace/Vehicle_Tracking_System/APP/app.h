/*
 * app.h
 *
 *  Created on: Dec 2, 2023
 *      Author: Omar
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include "../HAL/SIM900A_GSM/gsm.h"
#include "../HAL/Buzzer/buzzer.h"
#include "../HAL/LCD/lcd.h"
#include "../HAL/Sensors/MQ9/co_sensor.h"
#include "../MCAL/USART/usart.h"
#include "../MCAL/Timer/timer.h"
#include "../MCAL/GPIO/gpio.h"
#include "../MCAL/ADC/adc.h"

#include <util/delay.h>

#define DEF_CONFIRMATION_CODE       "VTS100"
#define NUM_BOOK_START_ADDR         0x000A
#define TIMER1_COMPARE_VALUE_3SEC   24576
#define LOCATION_HLINK_LENGTH   100
#define CONFIRM_CODE_LENGTH 7

typedef enum{
	GPS, GSM
}APP_UART_Access; 

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
boolean APP_isMsgReceived(char * sender_number, char * received_msg);

void APP_timerTickIncrement(void);

#endif /* APP_APP_H_ */
