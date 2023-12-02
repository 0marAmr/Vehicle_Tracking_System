/*
 * co_sensor.h
 *
 *  Created on: Dec 2, 2023
 *      Author: Omar
 */

#ifndef HAL_SENSORS_MQ9_CO_SENSOR_H_
#define HAL_SENSORS_MQ9_CO_SENSOR_H_

#include "../../../MCAL/ADC/adc.h"
#include "../../../MCAL/GPIO/gpio.h"

#define MQ_PORT_ID 				    PORTD_ID
#define MQ_PIN_ID					PIN3_ID
#define RL_VALUE                    10               //define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR         9.83            //(Sensor resistance in clean air)
#define SENSOR_OUTPUT_CHANNEL_ID 	ADC_ch0

									                                                 
uint8 MQ_getCOPercentage(float32);
float32 MQ_ReadSensor();
float32 MQ_resistanceCalculation(uint8);
float32 MQ_sensorCalibration();

#endif /* HAL_SENSORS_MQ9_CO_SENSOR_H_ */
