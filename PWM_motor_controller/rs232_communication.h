/*
 * rs232_communication.h
 *
 * Created: 25.03.2025 08:40:06
 *  Author: surmi
 */ 


#ifndef RS232_COMMUNICATION_H_
#define RS232_COMMUNICATION_H_
#include <avr/io.h>

#define RS232_RX_BUF_SIZE 32
#define RS232_RX_BUF_MASK (RS232_RX_BUF_SIZE - 1)
#define RS232_TX_BUF_SIZE 16
#define RS232_TX_BUF_MASK (RS232_TX_BUF_SIZE - 1)

// TODO: Describe functions 
void rs232_Init(uint32_t baud);
void rs232_Transmit_Byte(uint8_t data);
uint8_t rs232_Receive_Byte(void);
void rs232_Send_Data(uint8_t *data, uint8_t len);
uint8_t * rs232_Get_Frame(void);





#endif /* RS232_COMMUNICATION_H_ */