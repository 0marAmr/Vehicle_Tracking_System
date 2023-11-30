/******************************************************************************
 *
 * [FILE NAME]:     main.c
 *
 * [AUTHOR]:        Omar Amr
 *
 * [DATE]:          27-11-2023
 *
 * [Description]:   main function for Vehicle Tracking System
 *
 *******************************************************************************/
#include "HAL/SIM900A_GSM/gsm.h"
#include "MCAL/USART/usart.h"
#include "MCAL/Timer/timer.h"
#include "util/delay.h"
#include "HAL/LCD/lcd.h"

void APP_decodeMsg(char * recieved_msg){
// a new phone entry, send the current location, activate the buzzer for a given T seconds, or simply display a message on the LCD screen
}

char g_msg_buff [MSG_BUFFER_SIZE]; /*you might want to consider dynamic allocation*/
volatile char g_buff_index = 0;
boolean g_info_received_flag = FALSE;



int main(void){

	char msg_location [MSG_LOC_BUFFER_SIZE];
	char sender_number [DIAL_NO_LENGTH];
	char recieved_msg [REC_MSG_MAX_LENGTH];
	/********** Peripherals configurations **********/
	USART_ConfigType uart_config =
	{
			.usart_baud_rate = 9600,
			.usart_bit_mode = DATA_BITS_8,
			.usart_stop_bits = ONE_BIT,
			.usart_mode = ASYNCHRONOUS,
			.usart_parity = PARITY_DISABLED
	};

	USART_init(&uart_config);
	while(!SM_init(g_msg_buff)){
		LCD_displayString("Detecting GSM Module");
		LCD_clearScreen();
	}
	g_buff_index = 0; // instead of repeating this, consider calling some function of the APP layer from the GSM that flushes the buffer and sets the index to 0
	LCD_clearScreen();
	LCD_displayString("GSM Module Detected =)");

	while(1){
		if(g_info_received_flag){
			if(GSM_isMsgReceived(g_msg_buff, msg_location)){
				g_buff_index = 0;
				if(!GSM_readMsgContents(msg_location, g_msg_buff, sender_number, recieved_msg)){
					LCD_displayString("Msg Receiving Error !");
				}
				else {
					LCD_displayStringRowColumn(0, 0, "No.: ");
					LCD_displayString(sender_number);
					LCD_moveCursor(1,0);
					LCD_displayString(recieved_msg);
					APP_decodeMsg(recieved_msg);
					g_buff_index = 0;
				}
			}
			g_info_received_flag = FALSE;
		}
		// do sensor stuff here
	} /*end super loop*/
}

// there some method that i forgot the F about by calling the function using it pointer
ISR(USART_RXC_vect){
	g_msg_buff[g_buff_index] = UDR;	/* copy UDR (received value) to buffer */
	g_buff_index++;
	g_info_received_flag = TRUE;		/* flag for new message arrival */
}
