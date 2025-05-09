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
#include "command_parser.h"
#include "rs232_communication.h"



int main(void)
{	
	pwm_Init();
	rs232_Init(9600, timer_get_time_ms);
	pwm_Set_Duty(PWM_CHANNEL_2, 50);
	pwm_Set_Duty(PWM_CHANNEL_1, 50);
	pwm_Set_Duty(PWM_CHANNEL_3, 50);
	pwm_Set_Duty(PWM_CHANNEL_4, 50);
	pwm_Set_Duty(PWM_CHANNEL_5, 50);
	pwm_Set_Duty(PWM_CHANNEL_6, 50);

	// enable global interrupt 
	sei(); 
    /* Replace with your application code */
    while(1) 
    {
		//_delay_ms(50);
		Refresh_Channel_Settings();
		

    }
}


