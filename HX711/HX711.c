/*
 * HX711.c
 *
 *  Created on: Mar 19, 2024
 *      Author: Dominik
 */

#include "HX711.h"


void hx711_Init(HX711_t* hx711, GPIO_TypeDef *clk_port, uint16_t clk_pin, GPIO_TypeDef *data_port, uint16_t data_pin)
{
	hx711->clk_port = clk_port;
	hx711->data_port = data_port;
	hx711->clk_pin = clk_pin;
	hx711->data_pin = data_pin;
}
/*
 * When chip is powered up, on-chip power on
 *	rest circuitry will reset the chip.
 *	When PD_SCK Input is low, chip is in normal working mod
 */
void hx711_PowerUp(HX711_t* hx711)
{
	HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
}

/*
 * When PD_SCK pin changes from low to high
 * and stays at high for longer than 60μs, HX711
 * enters power down mode
 * After a reset or power-down event, input
 * selection is default to Channel A with a gain of 128
 */
void hx711_PowerDown(HX711_t* hx711)
{
	HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
}

/*
 * Input and gain selection is controlled by the
 * number of the input PD_SCK pulses
 */
void hx711_SetGain(HX711_t* hx711, uint8_t gain)
{
	hx711->gain = gain;
	hx711_PowerUp(hx711); //make sure hx711 is powered up


	switch(hx711->gain)
	{
	case ch_A_128:
		//TODO: check before if DOUT is ready?
		for(int i=0;i<(24+ch_A_128);i++)
		{
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		}
		break;
	case ch_A_64:
		for(int i=0;i<(24+ch_A_64);i++)
		{
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		}
		break;
	case ch_B_32:
		for(int i=0;i<(24+ch_B_32);i++)
		{
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		}
		break;
	default:
		for(int i=0;i<(24+ch_A_128);i++)
		{
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		}
	}

}


uint32_t hx711_Measure(HX711_t* hx711)
{
	uint32_t mass = 0;
	uint8_t data = 0;

	while(HAL_GPIO_ReadPin(hx711->data_port, hx711->data_pin))
	{
		/*
		 * When output data is not ready for retrieval,
		 * digital output pin DOUT is high.
		 * Wait for data transmission to be ready - data pin goes low
		 */
	}

	for(int i=24; i>0; i--)
	{
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		data = HAL_GPIO_ReadPin(hx711->data_port, hx711->data_pin);
		mass |= (uint32_t)(data << i);
	}

	switch(hx711->gain)
	{
	case ch_A_128:
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		break;
	case ch_A_64:
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		break;
	case ch_B_32:
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(hx711->clk_port, hx711->clk_pin, GPIO_PIN_RESET);
		break;
	}

	mass = mass ^ 0x800000;

	return mass;
}

void hx711_tare(HX711_t* hx711)
{
	int tare = 0;
	for(int i=0; i<10; i++)
	{
		tare += hx711_Measure(hx711);
		HAL_Delay(20);
	}
	hx711->tareVal = (float)(tare/10);
}

void hx711_Coefficient(HX711_t* hx711, float coeff)
{
	hx711->coeff = coeff;
}


float hx711_GetWeight(HX711_t* hx711)
{
	float mass = 0;
	int measure = hx711_Measure(hx711);

	mass = (float)((measure - hx711->tareVal) / hx711->coeff);

	return mass;
}

float hx711_GetWeightAverage(HX711_t* hx711, uint8_t nSamples, uint8_t SampleDlyms)
{
	float mass = 0;

	for(int i=0; i<nSamples; i++)
	{
		mass += hx711_GetWeight(hx711);
		HAL_Delay(SampleDlyms);
	}

	mass = mass/nSamples;

	return mass;
}


