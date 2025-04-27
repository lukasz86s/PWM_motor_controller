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
#define TIM1_PROCENT_TO_PWM(p) (uint16_t)((p * 1599UL) / 100)
#define TIM2_PROCENT_TO_PWM(p) (uint8_t)((p * 255U) / 100)

typedef void (*change_state_func)(void);

static volatile uint32_t system_time_ms = 0;
static PWM_Output_Mode_t current_pwm_mode = PWM_OUTPUT_MODE_0;

static volatile uint8_t enabled_soft_pwm_chnnels = 0;
static volatile uint8_t pwm_channel_duty_list[NUMBER_OF_CHANNELS] = {[0 ... (NUMBER_OF_CHANNELS-1)] = 0} ;
static volatile uint16_t pwm_pin_list[NUMBER_OF_CHANNELS] = {PWM_PINOUT_1, PWM_PINOUT_2, PWM_PINOUT_3, PWM_PINOUT_4, PWM_PINOUT_5, PWM_PINOUT_6}; 

// functions
void pwm_Timer1_Init(void);
void pwm_Timer2_Init(void);

// enabling hardware channels
void pwm_enable_OC1A(void);
void pwm_enable_OC1B(void);
void pwm_enable_OC2A(void);
void pwm_enable_OC2B(void);

change_state_func enable_hardware_timers_tab[] = { pwm_enable_OC1A, pwm_enable_OC1B, pwm_enable_OC2A, pwm_enable_OC2B };

// disabling hardware channels
void pwm_disable_OC1A(void);
void pwm_disable_OC1B(void);
void pwm_disable_OC2A(void);
void pwm_disable_OC2B(void);

change_state_func disable_hardware_timers_tab[] = { pwm_disable_OC1A, pwm_disable_OC1B, pwm_disable_OC2A, pwm_disable_OC2B };
void pwm_Change_Numbers_Soft_channels(uint8_t nr_channals);

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
	pwm_Timer1_Init();
	pwm_Timer2_Init();
	pwm_enable_OC1A();
	pwm_Change_Output_Mode(PWM_OUTPUT_MODE_0);

	
}


void pwm_Timer1_Init(void)
{
	// TODO: change to set on ocr
    // Fast PWM (mode 14: TOP = ICR1)
    TCCR1A = (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);

    // TOP  1599 ( 16MHz / 1,6k = 10KHz)
    ICR1 = ICR1_PWM_TOP;

    //Set duty to 0
    OCR1A = 0;
    OCR1B = 0;
}
void pwm_Timer2_Init(void)
{
    // Fast PWM (mode 3: TOP = 0xFF)
    TCCR2A = (1 << WGM20) | (1 << WGM21);
	// prescaler = 8 (16MHz / 8/ 256 = 7,8125KHz period)
	TCCR2B = (1 << CS21);
	//set duty to 0 
	OCR2A = 0;
	OCR2B = 0;
	
}

void pwm_enable_OC1A(void)
{
	DDRB |= (1 << DDB1);       
	TCCR1A |= (1 << COM1A1);   
}

void pwm_disable_OC1A(void)
{
	DDRB &= ~(1 << DDB1);
	TCCR1A &= ~(1 << COM1A1);
}

void pwm_enable_OC1B(void)
{
	DDRB |= (1 << DDB2);       
	TCCR1A |= (1 << COM1B1);   
}

void pwm_disable_OC1B()
{
	DDRB &= ~(1 << DDB2);
	TCCR1A &= ~(1 << COM1B1); 

}
void pwm_enable_OC2A(void)
{
	DDRB |= (1 << DDB3);       
	TCCR2A |= (1 << COM2A1);   
}

void pwm_disable_OC2A(void)
{
	DDRB &= ~(1 << DDB3);
	TCCR2A &= ~(1 << COM2A1) ; 
}

void pwm_enable_OC2B(void) 
{
	DDRD |= (1 << DDD3);       
	TCCR2A |= (1 << COM2B1);   
}

void pwm_disable_OC2B(void)
{
	DDRD &= ~(1 << DDD3);
	TCCR2A &= ~(1 << COM2B1); 
}

/**
 * @brief set numbers of all PWM
 * 
 * @param mode -> selcet PWM_OUTPUT_MODE_x (x = (0, 9)) 
 * 
 */
void pwm_Change_Output_Mode(PWM_Output_Mode_t mode)
{
	// end here if new mode is the same
	if(mode == current_pwm_mode) return;
	uint8_t soft_channels = 0;
	//TODO: move setting hardware pwm to separate function, consider the correctness of (mode + 1)
	if(mode > current_pwm_mode)
	{
		for(uint8_t i = current_pwm_mode; i < MAX_HARDWARE_PWM_CHANNELS; i++)
		{
			// enable hardware pwm
			enable_hardware_timers_tab[i]();
		}
	}else if(mode < current_pwm_mode)
	{
		for(uint8_t i = (mode + 1); i < MAX_HARDWARE_PWM_CHANNELS; i++)	//do not turn off the current == mode, so add 1
		{
			// disable hardware pwm
			disable_hardware_timers_tab[i]();
		}
	}
	// calculate number of soft pwm
	if(mode < MAX_HARDWARE_PWM_CHANNELS)
	soft_channels = 0;
	else
	soft_channels = (mode + 1) - MAX_HARDWARE_PWM_CHANNELS; // 0 count to , add 1
	// set number of soft channels
	pwm_Change_Numbers_Soft_channels(soft_channels);
	// assign new mode
	current_pwm_mode = mode;
	
}

/**
 * @brief set numbers of soft PWM
 * 
 * @param nr_channals -> number of channels that will be activated or deactivated, max 6 (pin PC0 to PC5)
 * 
 */
void pwm_Change_Numbers_Soft_channels(uint8_t nr_channals)
{
	
	if(nr_channals > 6) nr_channals = MAX_SOFT_PWM_CHANNELS;
	
	if(nr_channals > enabled_soft_pwm_chnnels)
	{
	
		for(uint8_t i = enabled_soft_pwm_chnnels ; i < nr_channals; i++)
		{
			DDRC |= (1 << i); // set pin as out
		}
	}else if(nr_channals < enabled_soft_pwm_chnnels)
	{
		
		for(uint8_t i = nr_channals ; i < enabled_soft_pwm_chnnels; i++)
		{
			DDRC &= ~(1 << i); // set pin as input
		}
	}
	// set new val 
	enabled_soft_pwm_chnnels = nr_channals;
}

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
	if(value > 100)value = 100;
	// calculate percent before atomic to reduce lock time
	if(channel < 2)
	{
		uint16_t tim1_percent = TIM1_PROCENT_TO_PWM(value);
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (channel == 0) OCR1A = tim1_percent;
			else OCR1B = tim1_percent;
		}
	}
	else if (channel == 2) OCR2A = TIM2_PROCENT_TO_PWM(value);
	else if (channel == 3) OCR2B = TIM2_PROCENT_TO_PWM(value);
	else
	{
		pwm_channel_duty_list[channel - MAX_HARDWARE_PWM_CHANNELS] = value;
	}
	
}

uint8_t pwm_Get_Duty(uint8_t channel)
{
	uint8_t channel_value = 0xFF; // max channel val = 100, more means error
	if(channel < 2)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (channel == 0) channel_value = OCR1A;
			else  channel_value = OCR1B ;
		}	
	}
	else if (channel == 2) channel_value = OCR2A ;
	else if (channel == 3) channel_value = OCR2B ;
	else
	{
		channel_value = pwm_channel_duty_list[channel - MAX_HARDWARE_PWM_CHANNELS] ;
	}
	
	return channel_value;
	
}


ISR(TIMER0_COMPA_vect){
	static uint8_t cnt ;
	static uint8_t time_sub_tick;
	for(uint8_t i = 0; i < enabled_soft_pwm_chnnels; i++)
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
