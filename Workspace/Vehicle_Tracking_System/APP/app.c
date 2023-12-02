/*
 * app.c
 *
 *  Created on: Dec 2, 2023
 *      Author: Omar
 */

#include "app.h"

char g_msg_buff [MSG_BUFFER_SIZE]; /*you might want to consider dynamic allocation*/
boolean g_info_received_flag = FALSE;
volatile char g_buff_index = 0;

/**/
uint16 g_numbers_end_addr;

uint8 g_timer1_tick = 0;

void APP_init(void){
    APP_switchUARTAccess(GSM);
    while(!GSM_init(g_msg_buff)){
		LCD_displayStringRowColumn(0,0,"Detecting GSM Module");
	}
    APP_flushBuffer();
    
    /*read values from EEPROM (by default end address is stored at addresses 6, 7 )*/
    g_numbers_end_addr = 6;   // EEPROM_read(7) +  
}

boolean APP_isMsgReceived(char * sender_number, char * received_msg){
	char msg_location [MSG_LOC_BUFFER_SIZE];
    if(g_info_received_flag){
        g_info_received_flag = FALSE;
        if(GSM_isMsgReceived(g_msg_buff, msg_location)){
		    APP_flushBuffer();
		    if(!GSM_readMsgContents(msg_location, g_msg_buff, sender_number, received_msg)){
		    	LCD_displayStringRowColumn(0,0,"Msg Receiving Error !");
		        APP_flushBuffer();
                return FALSE;
		    }
            else {
                LCD_displayStringRowColumn(0, 0, "No.: ");
		    	LCD_displayString(sender_number);
		    	LCD_displayStringRowColumn(1, 0,received_msg);
		        APP_flushBuffer();
                return TRUE;
            }
        }
    }
}

/*
D:(msg: "DISP ...") simply display a message on the LCD screen
E:(msg: "ENT VTS100") new phone entry 
L:(msg: "LOC")  send the current location
B:(msg: "BUZ")  activate the buzzer for 5 sec
C: (msg: "CNFG {old_code} {new_code}") change confirmation code
*/
void APP_decodeMsg(char * number, char * received_msg, TIMER_ConfigType * const timer1_configPtr){
    char * disp_msg;
    switch (received_msg[0]){
        case 'D':
            disp_msg = received_msg + 5;
            LCD_clearScreen();
            LCD_displayStringRowColumn(0,0,disp_msg);
            return;
        break;
        case 'E':
            if (APP_findNumber(number)){
                LCD_clearScreen();
                LCD_displayStringRowColumn(0,0,"Phone No Already Exists !");
            }
            else if(APP_codeCheck(received_msg)){
                APP_storeNewEntry(number);
                LCD_clearScreen();
                LCD_displayStringRowColumn(0,0,"No: ");
                LCD_displayString(number);
		    	LCD_displayStringRowColumn(1, 0,"Was Stored Successfully !");
            }
            else {
                LCD_clearScreen();
                LCD_displayStringRowColumn(0,0,"Wrong Confirmation Code !");
            }
            return;
        break;
    }

    if (!APP_findNumber(number)){
        LCD_clearScreen();
        LCD_displayStringRowColumn(0,0,"Unauthorized Access !");
        return;
    }

    switch (received_msg[0]){
        case 'L':
            APP_sendCoordinates(number);
        break;
        case 'B':
            BUZZER_start();
            TIMER_init(timer1_configPtr);
            while (g_timer1_tick < 1);
	        TIMER_deInit(TIMER1_ID);
            BUZZER_start();
            g_timer1_tick = 0;
        break;        
    }
}

void APP_flushBuffer(){
	memset(g_msg_buff,0,MSG_BUFFER_SIZE);
    g_buff_index = 0;
}


boolean APP_findNumber(char * number){
    char fetched_number [14];
    fetched_number[13] = '/0';
    /*read 13 characters from eeprom*/
    // return 0 if not found
}

boolean APP_strCmp(char * str1, char * str2){
    return strcmp(str1, str2) == 0;
}

boolean APP_codeCheck(char * message){
    char * code_to_check = message + 4;
    // char actual_code [6] =  EEPROM(fetch 1st 6 bytes)
    // return APP_strCmp(code_to_check, actual_code);
}

void APP_storeNewEntry(char * number){

}

void APP_switchUARTAccess(APP_UART_Access access_granted) {
    if (access_granted == GPS){
        // USART_setCallBackFunction();
        GPIO_writePin(PORTB_ID, PIN3_ID, LOGIC_LOW);
    }
    else if (access_granted == GSM){
        USART_setCallBackFunction(APP_bufferRecieve);
        GPIO_writePin(PORTB_ID, PIN3_ID, LOGIC_HIGH);
    }
    
}

void APP_sendCoordinates(char * number) {
    char msg_to_send [TRANS_MSG_MAX_LENGTH];
    APP_switchUARTAccess(GPS);
    /*GPS_getCoordinates(msg_to_send)*/
    APP_switchUARTAccess(GSM);
    GSM_sendMsg(number, msg_to_send, g_msg_buff, g_buff_index);
}

void APP_timerTickIncrement(void){
	g_timer1_tick++;
}

void APP_bufferRecieve(void){
    g_msg_buff[g_buff_index] = UDR;	    /* copy UDR (received value) to buffer */
	g_buff_index++;
	g_info_received_flag = TRUE;		/* flag for new message arrival */
}