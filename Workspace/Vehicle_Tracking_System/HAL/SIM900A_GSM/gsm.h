/*
 * gsm.h
 *
 *  Created on: Nov 29, 2023
 *      Author: Omar
 */

#ifndef GSM_H_
#define GSM_H_

#include "../../Utils/std_types.h"
#include "../../Utils/common_macros.h"
#include "../../MCAL/USART/usart.h"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MSG_BUFFER_SIZE 		256
#define MSG_LOC_BUFFER_SIZE 	4
#define DIAL_NO_LENGTH 			14
#define CONF_CODE_LENGTH 	    6
#define REC_MSG_MAX_LENGTH		16
#define LOCATION_HLINK_LENGTH   100
#define TRANS_MSG_MAX_LENGTH    150

/*"E0" turns off the echoing of characters. When echoing is off,
 * the module will not repeat back the characters it receives from the host (MCU).
\r: This is the carriage return character, indicating the end of the command.*/
#define NO_ECHO_CMD  		"ATE0\r"
#define TEXT_MODE_CMD		"AT+CMGF=1\r"
#define READ_MSG_CMD		"AT+CMGR="
#define SEND_MSG_CMD		"AT+CMGS="
#define DELETE_MSG_CMD		"AT+CMGD="
#define DEL_ALL_MSGS_CMD	"AT+CMGDA=\"DEL ALL\"\r"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

boolean GSM_init(char * message_buffer);
boolean GSM_isMsgReceived(char * message_buffer, char * message_location);
boolean GSM_readMsgContents(char * message_location, char * message_buffer, char * sender_number, char * recieved_message);
void GSM_sendMsg(char * number, char * message_to_send, char * message_buffer, char * buff_ptr);
void GSM_deleteMsg(char *message_location);
void GSM_deleteAllMsgs(void);

void GSM_StoreToBuff(char * buffer, const char * const_string);

#endif /* GSM_H_ */
