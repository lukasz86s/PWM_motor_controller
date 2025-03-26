/*
 * command_parser.c
 *
 * Created: 26.03.2025 10:14:23
 *  Author: surmi
 */ 
#include "component-version.h"
#include "rs232_communication.h"

#define MAX_DATA_LEN 10

typedef struct 
{	
	uint8_t function;
	uint8_t nr_of_channels;
	uint8_t channel_nr[MAX_DATA_LEN];
	uint8_t channel_vlue[MAX_DATA_LEN];
	}Frame_fields;
	
void parse_frame(void)
{
	char* data = rs232_Get_Frame();
	uint8_t len = data[0];
	// TODO: implement function check_crc(len, data)
}