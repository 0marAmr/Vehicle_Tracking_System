/*
 * app.c
 *
 *  Created on: Dec 2, 2023
 *      Author: Omar
 */

#include "app.h"

/*******************************************************************************
 *                     	   	  Global Variables                                 *
 *******************************************************************************/

char g_msg_buff [MSG_BUFFER_SIZE];        
char g_contact_number [DIAL_NO_LENGTH];
char g_confirmation_code [CONFIRM_CODE_LENGTH];
boolean g_info_received_flag = FALSE;
volatile char g_buff_index = 0;

/*from EEPROM*/
uint8 g_code_config_flag;
uint8 g_no_of_contacts;

uint8 g_timer1_tick = 0;

float32 g_Ro;


/*******************************************************************************
 *                      Functions Prototypes(Private)                          *
 *******************************************************************************/

static void APP_readConfirmCode(const char * conf_code);
static void APP_storeConfirmCode(const char * conf_code);
static void APP_sendCoordinates(char * number, char * special_message);
static void APP_switchUARTAccess(APP_UART_Access access_granted);
static void APP_storeNewEntry(char * number);
static boolean APP_codeCheck(char * code);
static boolean APP_findNumber(char * number);
static void APP_getContactNumber(uint8 contact_id);
static boolean APP_isSUbStr(const char *str, const char *sub) ;
static void APP_strCat(char * result, const char * str1, const char * str2);
static boolean APP_strCmp(char * str1, char * str2);
static void APP_flushBuffer();

/*******************************************************************************
 *                     		 Functions Definitions                             *
 *******************************************************************************/

void APP_init(void){
    APP_switchUARTAccess(GSM);
    LCD_clearScreen();
	LCD_displayStringRowColumn(0,0," Detecting GSM");
	LCD_displayStringRowColumn(1,0,"     Module");
    while(!GSM_init(g_msg_buff));
    APP_flushBuffer();
    g_info_received_flag = FALSE;
    /* get number of contacts saved in EEPROM (saved in address 7 by default)*/
    g_code_config_flag = EEPROM_read(6); 
    g_no_of_contacts = EEPROM_read(7); 

    if (!(g_code_config_flag == '$')){ /*special char that indicates the code has been configured*/
        APP_storeConfirmCode("VTS100");
    }
    
}

void APP_MQSenCalibration(){
    LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Calibrating MQ");
    g_Ro = MQ_sensorCalibration();
	LCD_displayStringRowColumn(0,0,"Calibration Done !");
}

boolean APP_isMsgReceived(char * sender_number, char * received_msg){
	char msg_location [MSG_LOC_BUFFER_SIZE];
    if(g_info_received_flag){
        g_info_received_flag = FALSE;
        if(GSM_isMsgReceived(g_msg_buff, msg_location)){
		    APP_flushBuffer();
		    if(!GSM_readMsgContents(msg_location, g_msg_buff, sender_number, received_msg)){
                LCD_clearScreen();
		    	LCD_displayStringRowColumn(0,0,"Msg Receiving Error !");
		        APP_flushBuffer();
                return FALSE;
		    }
            else {
                LCD_displayStringRowColumn(0, 0, "No.: ");
		    	LCD_displayString(sender_number);
		    	LCD_displayStringRowColumn(1, 0,received_msg);
		        APP_flushBuffer();
                GSM_deleteMsg(msg_location); /*preserve space*/
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
C:(msg: "CNFG {old_code} {new_code}") change confirmation code
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
		    	LCD_displayStringRowColumn(1, 0," Was Stored !");
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
            APP_sendCoordinates(number, "Location: ");
        break;
        case 'B':
            BUZZER_start();
            TIMER_init(timer1_configPtr);
            while (g_timer1_tick < 2);
	        TIMER_deInit(TIMER1_ID);
            BUZZER_stop();
            g_timer1_tick = 0;
        break;        
    }
}

static void APP_flushBuffer(){
	memset(g_msg_buff,0,MSG_BUFFER_SIZE);
    g_buff_index = 0;
}

static boolean APP_strCmp(char * str1, char * str2){
    return strcmp(str1, str2) == 0;
}

static void APP_strCat(char * result, const char * str1, const char * str2){
    char *resultPtr = result;
    while (*str1 != '\0') {
        *resultPtr = *str1;
        resultPtr++;
        str1++;
    }
    while (*str2 != '\0') {
        *resultPtr = *str2;
        resultPtr++;
        str2++;
    }
    *resultPtr = '\0';
}

static boolean APP_isSUbStr(const char *str, const char *sub){
    while (*str != '\0') {
        const char *subPtr = sub;
        // Check if the substring is found at the current position in the string
        while (*subPtr != '\0' && *str == *subPtr) {
            str++;
            subPtr++;
        }
        // If the substring is fully matched, return success
        if (*subPtr == '\0') {
            return TRUE; // Substring found
        }
        // Move back to the beginning of the substring and continue checking
        str = str - (subPtr - sub) + 1;
    }
    return FALSE;
}


/*Function that fetches a number from EEPROm
id = 1: starting from address 10 -> 18
id = 2: starting from address 19 -> 27
id = 3: starting from address 28 -> 36
..
*/
static void APP_getContactNumber(uint8 contact_id){
    uint16 i = NUM_BOOK_START_ADDR + (contact_id-1)*9;
    uint16 end_location = NUM_BOOK_START_ADDR + (contact_id)*9;
    uint8 j =0;
    char num [9];
    for ( ; i < end_location; i++, j++){
        num[j] = EEPROM_readByte(i);
    }
    APP_strCat(g_contact_number, "+01", num);
}

static boolean APP_findNumber(char * number){
    for (uint8 i = 1; i <= g_no_of_contacts; i++){
        APP_getContactNumber(i);
        if(APP_strCmp(g_contact_number, number)) {
            return TRUE;
        }
    }
    return FALSE;
}

static boolean APP_codeCheck(char * code){
    code += 4; // discard the first 4 characters
    return APP_strCmp(code, g_confirmation_code);
}

static void APP_storeNewEntry(char * number){
    uint16 i = g_no_of_contacts * 9 + NUM_BOOK_START_ADDR;
    uint8 j = 4;
    uint16 end_location = g_no_of_contacts * 9 + NUM_BOOK_START_ADDR + 8;
    for (; i < end_location; i++, j++){
        EEPROM_storeByte(i,number[j]);
    }
    g_no_of_contacts++;
    EEPROM_storeByte(7,g_no_of_contacts);

}

static void APP_switchUARTAccess(APP_UART_Access access_granted) {
    if (access_granted == GPS){
        USART_setCallBackFunction();
        GPIO_writePin(PORTB_ID, PIN3_ID, LOGIC_LOW);
    }
    else if (access_granted == GSM){
        USART_setCallBackFunction(APP_bufferRecieve);
        GPIO_writePin(PORTB_ID, PIN3_ID, LOGIC_HIGH);
    }
    
}

static void APP_sendCoordinates(char * number, char * special_message) {
    char msg_to_send [TRANS_MSG_MAX_LENGTH];
    char location_hyperlink [LOCATION_HLINK_LENGTH];
    APP_switchUARTAccess(GPS);
    /*GPS_getCoordinates(location_hyperlink)*/
    APP_switchUARTAccess(GSM);
    APP_strCat(msg_to_send, special_message, location_hyperlink);
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

uint8 APP_getCOVal(){
    return (MQ_getCOPercentage(MQ_ReadSensor()/g_Ro));
}

boolean APP_COThresholdExceeded(){
    return MQ_getDigIP();
}

void APP_fireEmergency(TIMER_ConfigType * const timer1_configPtr){
    uint8 i;
    TIMER_init(timer1_configPtr);
    while (g_timer1_tick < 1); // wait for 3 seconds and check again for CO Threshold
	TIMER_deInit(TIMER1_ID);
    g_timer1_tick = 0;
    if(!APP_COThresholdExceeded()){
        return; 
    }
    for ( i = 1; i <= g_no_of_contacts; i++){
        APP_getContactNumber(i);
        APP_sendCoordinates(g_contact_number,"Fire Emergency: ");
    }
    BUZZER_start();
    while (APP_COThresholdExceeded());
    BUZZER_stop();
}

static void APP_storeConfirmCode(const char * conf_code){
    for(uint8 i = 0; i < CONFIRM_CODE_LENGTH; i++) {
        EEPROM_storeByte(i, conf_code[i]);
    }
    APP_strCat(g_confirmation_code, conf_code, "");
}

static void APP_readConfirmCode(const char * conf_code){
    for(uint8 i = 0; i < CONFIRM_CODE_LENGTH-1; i++) {
        g_confirmation_code[i] = EEPROM_readByte(i);
    }
    g_confirmation_code[CONFIRM_CODE_LENGTH-1] = '\0';
}