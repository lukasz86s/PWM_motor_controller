/*
 * PWM_motor_controller.c
 *
 * Created: 22.03.2025 16:59:25
 * Author : surmi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "pwm_controller.h"
#include "rs232_communication.h"


char test_data[13] = {'a', 'b', 'c', 'd', 'e', 'f', 'g' };
int main(void)
{	
	pwm_Init();
	rs232_Init(9600);
	// set output on pins
	DDRB |= ((1 << PB2) | (1 << PB1)| (1 << PB0));

	// enable global interrupt 
	sei(); 
    /* Replace with your application code */
	uint8_t test = 0;

    while (1) 
    {
		pwm_Set_Duty(PWM_CHANNEL_3, test);
		pwm_Set_Duty(PWM_CHANNEL_2, 100 - test);
		pwm_Set_Duty(PWM_CHANNEL_1, 50 + test/2);
		_delay_ms(1000);
		test += 10;
		if(test > 100)test = 0;
		rs232_Send_Data("test", 4);
		
		
    }
}


