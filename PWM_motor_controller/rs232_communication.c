/*
 * rs232_communication.c
 *
 * Created: 25.03.2025 08:39:40
 *  Author: surmi
 */ 
#include "rs232_communication.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

void rs232_Init(uint32_t baud)
{	
	uint16_t _ubrr = F_CPU / 16 / (baud-1);
	
	UBRR0H |= (uint8_t)(_ubrr >> 8);
	UBRR0L |= (uint8_t) _ubrr;
	//enable revevier and transmitter
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	//set frame format 8data , 1 stop bit
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//enable iterrupt from Rx  and Tx
	//UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0);
	
	
}
void rs232_Transmit_Byte(uint8_t data)
{
	while (!(UCSR0A & (1<<UDRE0))); // wait until ready to send
	UDR0 = data;
}
uint8_t rs232_Receive_Byte(void)
{
    while (!(UCSR0A & (1<<RXC0))); // wait to receive end
    return UDR0;                  	
}

ISR(USART_RX_vect)
{
	
}