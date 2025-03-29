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

volatile uint8_t RxBuf[RS232_RX_BUF_SIZE];
volatile uint8_t RxHead;
volatile uint8_t RxTail;
uint8_t frame_buff[RS232_RX_BUF_SIZE/2];

volatile uint8_t TxBuf[RS232_TX_BUF_SIZE];
volatile uint8_t TxHead;
volatile uint8_t TxTail;


volatile uint8_t new_data_flag ; 

uint8_t getc_from_rx_buff(void);
void putc_into_tx_buff(uint8_t data);

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
	UCSR0B |= (1<<RXCIE0);
	
	
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
	uint8_t temp_head;
	uint8_t data;
	data = UDR0;
	temp_head = (RxHead + 1) & RS232_RX_BUF_MASK;
	if(temp_head == RxTail)
	{
		// TODO: overwrite data error handler
	}else
	{
		RxHead = temp_head;
		RxBuf[RxHead] = data;
		if(!new_data_flag) new_data_flag = 1; 
	}
	
}


uint8_t getc_from_rx_buff(void)
{
	if( RxHead == RxTail) return 0;
	RxTail = (RxTail + 1) & RS232_RX_BUF_MASK;
	return RxBuf[RxTail];
}

void putc_into_tx_buff(uint8_t data)
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

void rs232_Send_Data(uint8_t *data, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++)
	{
		putc_into_tx_buff(data[i]);
		//start transmmit
		UCSR0B |= (1<<UDRIE0);
	}
}

uint8_t* rs232_Get_Frame(void)
{	
	// if there is no new data return 
	if( !new_data_flag)
		return 0;
	new_data_flag = 0;
	//buffer length counter
	uint8_t invalid_data_cnt = RS232_RX_BUF_SIZE;
	// wait for the start of a frame no longer than the buffer length
	while(getc_from_rx_buff() != 0x55  )
	{
		if(invalid_data_cnt--)return 0;
	}
	//get lenght of frame
	uint8_t frame_length = getc_from_rx_buff();
	//copy frame with lenght on begin
	frame_buff[0] = frame_length;
	for(uint8_t i = 1; i < frame_length; i++) 
	{
		//offset 1 , frame_buff[0] is occupied
		frame_buff[i] = getc_from_rx_buff();
	}
    return frame_buff;
}