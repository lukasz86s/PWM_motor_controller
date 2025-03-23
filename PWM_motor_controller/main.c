/*
 * PWM_motor_controller.c
 *
 * Created: 22.03.2025 16:59:25
 * Author : surmi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#define F_CPU 16000000UL

volatile uint8_t pwm_list[6] = {[0 ... 5] = 50} ;
volatile uint8_t MAX_PWM = 3;
int main(void)
{	
	//set CTC mode on timer2
	TCCR2A |= (1 << WGM21);		// CTC mode
	TCCR2B |= (1 << CS21);		// preskaler 8
	// set OCR2A to 199 (16Mzh / 8 / 200 = 0.1ms period)
	OCR2A = 199;
	// compare intterupt enable
	TIMSK2 |= (1 << OCIE2A);
	// set output on pins
	DDRB |= ((1 << PB2) | (1 << PB1)| (1 << PB0));


	// enable global interrupt 
	sei(); 
    /* Replace with your application code */
    while (1) 
    {
		pwm_list[2] = 85;
		pwm_list[0] = 25;

		
    }
}

ISR(TIMER2_COMPA_vect){
	static uint8_t cnt ;
	for(uint8_t i = 0; i < MAX_PWM; i++)
	{
		if(pwm_list[i] > cnt) PORTB |= (1 << i); else PORTB &= ~(1 << i);
	}
	
	cnt ++;
	if (cnt >= 100) cnt = 0;
}

