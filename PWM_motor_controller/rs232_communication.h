/*
 * rs232_communication.h
 *
 * Created: 25.03.2025 08:40:06
 *  Author: surmi
 */ 


#ifndef RS232_COMMUNICATION_H_
#define RS232_COMMUNICATION_H_
#include <avr/io.h>


void rs232_Init(uint32_t baud);
void rs232_Transmit_Byte(uint8_t data);
uint8_t rs232_Receive_Byte(void);





#endif /* RS232_COMMUNICATION_H_ */