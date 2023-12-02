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
#include "APP/app.h"






int main(void){

	char sender_number [DIAL_NO_LENGTH];
	char received_msg [REC_MSG_MAX_LENGTH];
	/********** Peripherals configurations **********/
	USART_ConfigType uart_config =
	{
			.usart_baud_rate = 9600,
			.usart_bit_mode = DATA_BITS_8,
			.usart_stop_bits = ONE_BIT,
			.usart_mode = ASYNCHRONOUS,
			.usart_parity = PARITY_DISABLED
	};
	
	TIMER_ConfigType timer1_config = {
			.timer_id = TIMER1_ID,
			.timer_mode = COMPARE_MODE,
			.timer_mode_data.ctc_compare_value = TIMER1_COMPARE_VALUE_5SEC,
			.timer_prescaler.timer1 = TIMER1_F_CPU_1024,
			.timer_ocx_pin_behavior = DISCONNECT_OCX,
	};

	/*set timer1 call back function*/
	TIMER_setCallBackFunc(TIMER1_ID, APP_timerTickIncrement);

	USART_init(&uart_config);
	BUZZER_init();
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT); /*Initialize Relay Pin*/
	APP_init();

	LCD_clearScreen();
	LCD_displayString("GSM Module Detected =)");

	while(1){
		if (APP_isMsgReceived(sender_number, received_msg)){
			APP_decodeMsg(sender_number, received_msg, &timer1_config);
		}
		
		// do sensor stuff here
	} /*end super loop*/
}
