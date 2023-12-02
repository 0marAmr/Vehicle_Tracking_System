/*
 * gsm.c
 *
 *  Created on: Nov 29, 2023
 *      Author: Omar
 */

#include "gsm.h"


boolean GSM_init(char * message_buffer){
	USART_sendString(NO_ECHO_CMD);
	_delay_ms(500); /*half a second response time*/
	if(strstr(message_buffer,"OK")){ /*write your own GSM_subStrfind() function*/
		USART_sendString(TEXT_MODE_CMD);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

boolean GSM_isMsgReceived(char * message_buffer, char * message_location){
	char buf_ptr = 0, i = 0;
	if(!strstr(message_buffer,"CMTI:")){
		return FALSE;
	}
	else{
		while(message_buffer[buf_ptr]!= ','){
				buf_ptr++;
		}

		while(message_buffer[buf_ptr++]!= '\r'){
			message_location[i++]= message_buffer[buf_ptr];				      /* copy location of received message where it is stored */
		}
		message_location[i] = '\0';
		return TRUE;
	}
}

boolean GSM_readMsgContents(char * message_location, char * message_buffer, char * sender_number, char * recieved_message){
	char read_command [13]; /*13 character for the command and the location*/
	char buf_ptr = 0, i = 0;

	sprintf(read_command, "%s%s\r", READ_MSG_CMD, message_location);
	USART_sendString(read_command);
	_delay_ms(1000); /*wait a second for buffer to fill up (might consider reducing after testing)*/
	if(!strstr(message_buffer,"+CMGR")){
		return FALSE;
	}
	else {
		while(message_buffer[buf_ptr]!= ','){ /*to get mobile number*/
			buf_ptr++;
		}
		buf_ptr +=2;
		/*extract phone number*/
		for(i =0; i<DIAL_NO_LENGTH-1; i++){
			sender_number[i] = message_buffer[buf_ptr];
			buf_ptr++;
		}
		sender_number[i] = '\0';
		while(message_buffer[buf_ptr++]!= '\n'); /*to get the message*/
		i = 0;
		while ( (i < REC_MSG_MAX_LENGTH) && (message_buffer[buf_ptr] != '\r')){
			recieved_message[i] = message_buffer[buf_ptr];
			i++;
			buf_ptr++;
		}
		recieved_message[i] = '\0';
		return TRUE;
	}
}

void GSM_sendMsg(char * number, char * message_to_send, char * message_buffer, char * buff_ptr){
	char send_msg_command[23];
	sprintf(send_msg_command,"%s\"%s\"\r", SEND_MSG_CMD, number);
	USART_sendString(send_msg_command);
	_delay_ms(500);
	while(TRUE)
	{
		if(message_buffer[*buff_ptr]==0x3e) /*'>' character*/
		{
		   buff_ptr = 0;
		   memset(message_buffer,0,MSG_BUFFER_SIZE);
		   USART_sendString(message_to_send);
		   USART_sendByte(0x1a); /* send Ctrl+Z */
		   break;
		}
		(*buff_ptr)++;
	}
}

void GSM_deleteMsg(char * message_location){
	char delete_command[13];
	sprintf(delete_command,"%s%s\r",READ_MSG_CMD, message_location);
	USART_sendString(delete_command);

}

void GSM_deleteAllMsgs(){
	USART_sendString(DEL_ALL_MSGS_CMD);
}




