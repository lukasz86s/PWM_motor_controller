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

volatile char RxBuf[RS232_RX_BUF_SIZE];
volatile uint8_t RxHead;
volatile uint8_t RxTail;

volatile char TxBuf[RS232_TX_BUF_SIZE];
volatile uint8_t TxHead;
volatile uint8_t TxTail;

char getc_from_rx_buff(void);
void putc_into_tx_buff(char data);

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
	UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0);
	
	
}
void rs232_Transmit_Byte(char data)
{
	while (!(UCSR0A & (1<<UDRE0))); // wait until ready to send
	UDR0 = data;
}
char rs232_Receive_Byte(void)
{
    while (!(UCSR0A & (1<<RXC0))); // wait to receive end
    return UDR0;                  	
}

ISR(USART_RX_vect)
{
	uint8_t temp_head;
	char data;
	data = UDR0;
	temp_head = (RxHead + 1) & RS232_RX_BUF_MASK;
	if(temp_head == RxTail)
	{
		// TODO: overwrite data error handler
	}else
	{
		RxHead = temp_head;
		RxBuf[RxHead] = data;
	}
	
}

void rs232_Set_Tx_Flag(void)
{
	UCSR0B |= (1<<UDRIE0);
}

char getc_from_rx_buff(void)
{
	if( RxHead == RxTail)
		return 0;
	RxTail = (RxTail + 1) & RS232_RX_BUF_MASK;
	return RxBuf[RxTail];
}

void putc_into_tx_buff(char data)
{
	uint8_t temp_head;
	//calculate new head position 
	temp_head = (TxHead + 1) & RS232_TX_BUF_MASK;
	//stop as long as buffer full
	while( temp_head == TxTail){};	  
	// write data to buffer 
	TxBuf[temp_head] = data;
	TxHead = temp_head;
	
}

ISR(USART_UDRE_vect)
{
	if(TxHead != TxTail){
		//calculate new position
		TxTail = (TxTail + 1) & RS232_TX_BUF_MASK;
		//read form buffer 
		UDR0 = TxBuf[TxTail];

	}else
	{
		// buffer empty, disable interrupt on flag
		UCSR0B &= ~(1<<UDRIE0);
	}
}

void rs232_Send_Data(char *data, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		putc_into_tx_buff(data[i]);
	}
	//start transmmit
	rs232_Set_Tx_Flag();
	
}