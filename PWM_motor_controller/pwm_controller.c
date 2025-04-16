/*
 * pwm_controller.c
 *
 * Created: 24.03.2025 09:38:59
 *  Author: surmi
 */ 
#include "pwm_controller.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define MAX_PWM_CHANNELS 10
#define MAX_HARDWARE_PWM_CHANNELS 4
#define MAX_SOFT_PWM_CHANNELS MAX_PWM_CHANNELS - MAX_HARDWARE_PWM_CHANNELS	// 4 hardware pwm
#define ICR1_PWM_TOP 1599

static volatile uint32_t system_time_ms = 0;

static volatile uint8_t enbled_soft_pwm_chnnels = 0;
static volatile uint8_t pwm_channel_duty_list[NUMBER_OF_CHANNELS] = {[0 ... (NUMBER_OF_CHANNELS-1)] = 0} ;
static volatile uint16_t pwm_pin_list[NUMBER_OF_CHANNELS] = {PWM_PINOUT_1, PWM_PINOUT_2, PWM_PINOUT_3, PWM_PINOUT_4, PWM_PINOUT_5, PWM_PINOUT_6}; 
void pwm_Timer1_enable(void);
/**
 * @brief calculate crc16
 * 
 * @param *data -> pointer on first element of data to calculate
 * @param length -> numbers of bytes to calculate 
 * 
 * @note need include <avr/io.h>
 * @attention change TCCR0A, TCCR0B, TIMSK0, OCR0A  .
 */
void pwm_Init (void)
{	
	//Timer0 used for measuring system time and soft PWMs
	//set CTC mode on timer0
	TCCR0A |= (1 << WGM01);		// CTC mode
	TCCR0B |= (1 << CS01);		// prescaler 8
	// set OCR0A to 99 (16Mzh / 8 / 100 = 0.05ms )
	OCR0A = 99;
	// compare interrupt enable
	TIMSK0 |= (1 << OCIE0A);
	// default 2 channels pwm PB1, PB2
	pwm_Timer1_enable();
	
}
void pwm_Timer1_enable(void)
{
    DDRB |= (1 << PB1) | (1 << PB2); // OC1A -> PB1, OC1B -> PB2
	// TODO: change to set on ocr
    // Fast PWM (mode 14: TOP = ICR1)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);

    // TOP  1599 ( 16MHz / 1,6k = 10KHz)
    ICR1 = ICR1_PWM_TOP;

    //Set duty to 0
    OCR1A = 0;
    OCR1B = 0;
}
void pwm_Timer2_enable(void)
{
	DDRB |= (1 << PB3) | (1 << PD3);  // OC2A -> PB3 , OC2B -> PD3
    // Fast PWM (mode 3: TOP = 0xFF)
    TCCR2A = (1 << WGM20) | (1 << WGM21) |
			 (1 << COM2A1) | (1 << COM2B1);
	// prescaller = 8 (16MHz / 8/ 256 = 7,8125KHz period)
	TCCR2B = (1 << CS21);
	//set duty to 0 
	OCR2A = 0;
	OCR2B = 0;
	
}
/**
 * @brief set numbers of soft PWM
 * 
 * @param nr_channals -> number of channels that will be activated, max 6 (pin PC0 to PC5)
 * 
 */
void pwm_Soft_enable(uint8_t nr_channals)
{
	
	if(nr_channals > 6) enbled_soft_pwm_chnnels = MAX_SOFT_PWM_CHANNELS;
	else enbled_soft_pwm_chnnels = nr_channals;
	for(uint8_t i = 0 ; i < enbled_soft_pwm_chnnels; i++)
	{
		DDRC |= (1 << i); // set pin as out
	}
}
// TODO: Add disable functions for soft and hardware pwms
/**
 * @brief counts system time since startup in milliseconds
 * 
 * @return time in milliseconds 
 * 
 *@note: maximum time at 32 bits is about 50 days.
 * for continuous operation system should be increased to 64 bits
 */
uint32_t timer_get_time_ms(void)
{
	uint32_t time;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		time = system_time_ms;
	}
	return time;
}

void pwm_Set_Duty(PWM_Channel_t channel, uint8_t value)
{
	if (channel == 0) OCR1A = value;
	else if (channel == 1) OCR1B = value;
	else if (channel == 2) OCR2A = value;
	else if (channel == 3) OCR2B = value;
	else
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			pwm_channel_duty_list[channel - MAX_HARDWARE_PWM_CHANNELS] = value;
		}
	}
}

ISR(TIMER0_COMPA_vect){
	static uint8_t cnt ;
	static uint8_t time_sub_tick;
	for(uint8_t i = 0; i < enbled_soft_pwm_chnnels; i++)
	{
		if(pwm_channel_duty_list[i] > cnt) PORTC |= (1 << i); else PORTC &= ~(1 << i);
	}
	if (++cnt >= 100) cnt = 0;
	if(++time_sub_tick >= 20)
	{
		time_sub_tick = 0;
		system_time_ms++;
	}
	
}
