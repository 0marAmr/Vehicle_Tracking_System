/*
 * co_sensor.c
 *
 *  Created on: Dec 2, 2023
 *      Author: Omar
 */


#include <util/delay.h>
#include "co_sensor.h"

void MQ_init(void){
    GPIO_setupPinDirection(MQ_PORT_ID, MQ_PIN_ID, PIN_INPUT);
    GPIO_setupPinDirection(PORTA_ID, SENSOR_OUTPUT_CHANNEL_ID, PIN_INPUT);

}

boolean MQ_getDigIP(void){
    return GPIO_readPin(MQ_PORT_ID, MQ_PIN_ID);
}

float32 g_COCurve[3] ={2.3,0.20,-0.45};  //two points from smoke COCurve are taken point1:(200,1.6) point2(10000,0.62) 
                                        //take log of each point (lg200, lg3.4)=(2.3,0.53)  (lg10000,lg0.63)=(4,-0.20)
                                        //find the slope using these points. take point1 as reference   
                                        //data format:{ x, y, slope};

// sensor and load resistor forms a voltage divider. so using analog value and load value
// we will find sensor resistor.

static ResistanceCalculation(uint8 raw_adc){ 
    return ( ((float32)RL_VALUE*(1023-raw_adc)/raw_adc));   // we will find sensor resistor.
}

float32 MQ_sensorCalibration(){
    uint8 i;                                   // This function assumes that sensor is in clean air.
    float32 val=0;
    
    for (i=0;i<50;i++){                   //take multiple samples and calculate the average value
        val += ResistanceCalculation(ADC_readChannel(SENSOR_OUTPUT_CHANNEL_ID));
        _delay_ms(500);
    }

    val = val/50;                  
    val = val/RO_CLEAN_AIR_FACTOR;           //divided by RO_CLEAN_AIR_FACTOR yields the Ro according to the chart in the datasheet 
    
    return val;
}

float32 MQ_readSensor(){
    uint8 i;
    float32 rs=0;
    for (i=0;i<5;i++) {                                 // take multiple readings and average it.
        rs += ResistanceCalculation(ADC_readChannel(SENSOR_OUTPUT_CHANNEL_ID));   // rs changes according to gas concentration.
        _delay_ms(50);
    }

    rs = rs/5;
    
    return rs;  
}


//Using slope,ratio(y2) and another point(x1,y1) on line we will find
// gas concentration(x2) using x2 = [((y2-y1)/slope)+x1]
// as in curves are on logarithmic coordinate, power of 10 is taken to convert result to non-logarithmic.
uint8  MQ_getCOPercentage(float32 rs_ro_ratio){                                                                          
    return (pow(10,( ((log(rs_ro_ratio)-g_COCurve[1])/g_COCurve[2]) + g_COCurve[0]))); 
}