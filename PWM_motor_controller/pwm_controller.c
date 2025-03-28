/*
 * pwm_controller.c
 *
 * Created: 24.03.2025 09:38:59
 *  Author: surmi
 */ 
#include "pwm_controller.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t enbled_pwm_chnnels = 3;
volatile uint8_t pwm_channel_duty_list[NUMBER_OF_CHANNELS] = {[0 ... (NUMBER_OF_CHANNELS-1)] = 0} ;
volatile uint16_t pwm_pin_list[NUMBER_OF_CHANNELS] = {PWM_PINOUT_1, PWM_PINOUT_2, PWM_PINOUT_3, PWM_PINOUT_4, PWM_PINOUT_5, PWM_PINOUT_6}; 

/**
 * @brief calculate crc16
 * 
 * @param *data -> pointer on first element of data to calculate
 * @param length -> numbers of bytes to calculate 
 * 
 * @note need include <avr/io.h>
 * @attention change TCCR2A, TCCR2B, TIMSK2, OCR2A  .
 */
void pwm_Init (void)
{
	//set CTC mode on timer2
	TCCR2A |= (1 << WGM21);		// CTC mode
	TCCR2B |= (1 << CS21);		// preskaler 8
	// set OCR2A to 199 (16Mzh / 8 / 200 = 0.1ms period)
	OCR2A = 199;
	// compare intterupt enable
	TIMSK2 |= (1 << OCIE2A);
	
}
void pwm_Set_Duty(PWM_Channel_t channel, uint8_t value){
	pwm_channel_duty_list[channel] = value;
}

ISR(TIMER2_COMPA_vect){
	static uint8_t cnt ;
	for(uint8_t i = 0; i < enbled_pwm_chnnels; i++)
	{
		if(pwm_channel_duty_list[i] > cnt) PORTB |= (1 << i); else PORTB &= ~(1 << i);
	}
	
	cnt ++;
	if (cnt >= 100) cnt = 0;
}
