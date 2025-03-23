/*
 * PWM_motor_controller.c
 *
 * Created: 22.03.2025 16:59:25
 * Author : surmi
 */ 

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL


int main(void)
{	
	int a = 0;
	DDRB |= (1 << PB2); 
    /* Replace with your application code */
    while (1) 
    {
		PORTB ^= (1 << PB2); // Zmiana stanu pinu PB2
		_delay_ms(400);
		a++;
		if(a > 10) a += 2;
    }
}

