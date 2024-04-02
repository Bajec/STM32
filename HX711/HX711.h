/*
 * HX711.h
 *
 *  Created on: Mar 19, 2024
 *      Author: Dominik
 */

#ifndef INC_HX711_H_
#define INC_HX711_H_

#include "main.h"

typedef struct{
	GPIO_TypeDef* 	clk_port;
	GPIO_TypeDef* 	data_port;
	uint16_t		clk_pin;
	uint16_t		data_pin;

	uint8_t			gain;
	float			tareVal;
	float			coeff;

}HX711_t;

typedef enum{
	ch_A_128 = 1,
	ch_A_64 = 2,
	ch_B_32 = 3,
}gain;


void hx711_Init(HX711_t* hx711, GPIO_TypeDef *clk_port, uint16_t clk_pin, GPIO_TypeDef *data_port, uint16_t data_pin);
void hx711_PowerUp(HX711_t* hx711);
void hx711_PowerDown(HX711_t* hx711);
void hx711_SetGain(HX711_t* hx711, uint8_t gain);
uint32_t hx711_Measure(HX711_t* hx711);
void hx711_tare(HX711_t* hx711);
void hx711_Coefficient(HX711_t* hx711, float coeff);
float hx711_GetWeight(HX711_t* hx711);
float hx711_GetWeightAverage(HX711_t* hx711, uint8_t nSamples, uint8_t SampleDlyms);

#endif /* INC_HX711_H_ */
