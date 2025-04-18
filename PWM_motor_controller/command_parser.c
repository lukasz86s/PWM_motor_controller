/*
 * command_parser.c
 *
 * Created: 26.03.2025 10:14:23
 *  Author: surmi
 */ 
#include "command_parser.h"
#include "pwm_controller.h"
#include "rs232_communication.h"
#include <util/crc16.h>

#ifndef NULL 
	#define NULL 0
#endif

#ifndef MAX_PWM_CHANNELS
	#define MAX_PWM_CHANNELS 10
#endif

#define CRC_BYTES_LEN 2
#define CONST_FRAME_LEN 3

#define PING 0
#define RESPONSE_STATUS_LEN 5
// TODO: create function crate status frame 
const uint8_t empty_buffer_error_frame[] = {0x55, 0x4, EMPTY_BUFFER_ERROR, 0xD0, 0xA2 };
const uint8_t crc16_check_error_frame[] = {0x55, 0x4,CRC16_CHECK_ERROR, 0xD1, 0xE2};
const uint8_t data_len_error_frame[] = {0x55, 0x4,DATA_LEN_ERROR, 0x11, 0x23 };
const uint8_t ping_frame[] = {0x55, 0x4,PING_CMD, 0x73, 0xC3};




typedef enum{
	FRAME_LEN,
	CMD,
	CHANNELS_TO_SET,
	FIRST_CHANNELS_DATA,
	}Frame_Fields_Enum;

typedef struct 
{	
	uint8_t function;
	uint8_t channels_to_set;
	uint8_t channel_number[MAX_PWM_CHANNELS];
	uint8_t channel_value[MAX_PWM_CHANNELS];
	}Frame_Fields;
	
Frame_Fields PWM_Channel_Cofnig;

/**
 * @brief calculate crc16
 * 
 * @param *data -> pointer on first element of data to calculate
 * @param length -> numbers of bytes to calculate 
 * 
 * @note need include <util/crc16.h> to work
 */
uint16_t calculate_crc(const uint8_t *data, uint8_t length)
{
	uint16_t crc = 0xFFFF; // initializing crc val
	for(uint8_t i = 0; i < length; i++)
	{
		crc = _crc16_update(crc, data[i]);
	}
	return crc;
}
	
uint8_t parse_frame(void)
{
	//TODO: change to const data
	uint8_t* data = rs232_Get_Frame();
	if(data == NULL)
		return EMPTY_BUFFER_ERROR;
		
	uint8_t len = data[FRAME_LEN];
	uint8_t len_without_crc = len - CRC_BYTES_LEN;
	uint16_t crc_score = calculate_crc(data, len_without_crc);
	uint16_t crc = (data[len_without_crc] << 8) | data[len_without_crc + 1];
	if(crc != crc_score)
		return CRC16_CHECK_ERROR;
		
	PWM_Channel_Cofnig.function = data[CMD];
	if(PWM_Channel_Cofnig.function == PING_CMD) return NO_ERROR ; // no need to copy empty config 
	PWM_Channel_Cofnig.channels_to_set = data[CHANNELS_TO_SET];
	if(len - CONST_FRAME_LEN - CRC_BYTES_LEN != data[CHANNELS_TO_SET]*2)
		return DATA_LEN_ERROR;
	
	for(uint8_t channel_config_idx = 0; channel_config_idx < PWM_Channel_Cofnig.channels_to_set; channel_config_idx++)
	{	
		PWM_Channel_Cofnig.channel_number[channel_config_idx]  = data[FIRST_CHANNELS_DATA + 2*channel_config_idx];
		PWM_Channel_Cofnig.channel_value[channel_config_idx ] = data[FIRST_CHANNELS_DATA + 2*channel_config_idx +1];
	}
	return NO_ERROR;
	
}

/**
 * @brief sending extracted data from received frame 
 * 
 * @param stats -> struct holding extracted data form frame
 * @return void 
 * 
 * @note only to test.correctness of transmitted data
 */
void print_pwm_stat(Frame_Fields * stats)
{
	rs232_Send_Data(&stats->function, 1);
	rs232_Send_Data(&stats->channels_to_set, 1);
	rs232_Send_Data(stats->channel_number, stats->channels_to_set);
	rs232_Send_Data(stats->channel_value, stats->channels_to_set);
	
}

/**
 * @brief 
 * @note 
 */
	void Refresh_Channel_Settings(void)
{
	uint8_t status = parse_frame();
	if(status == EMPTY_BUFFER_ERROR)
	{	
		return;
	}
	else if(status == CRC16_CHECK_ERROR)
	{
		rs232_Send_Data(crc16_check_error_frame, RESPONSE_STATUS_LEN );
		return;
		
	}
	else if(status == DATA_LEN_ERROR)
	{
		rs232_Send_Data(data_len_error_frame, RESPONSE_STATUS_LEN );
		return;
	}
	switch(PWM_Channel_Cofnig.function)
	{
		case WRITE_CMD:
			{
				pwm_Set_Duty(PWM_Channel_Cofnig.channel_number[0], PWM_Channel_Cofnig.channel_value[0]);
				break;				
			}
		case READ_CMD:
			break;
		case WRITE_MANY_CMD:
			{
				for(uint8_t i=0; i<PWM_Channel_Cofnig.channels_to_set; i++)
					{
						pwm_Set_Duty(PWM_Channel_Cofnig.channel_number[i], PWM_Channel_Cofnig.channel_value[i] );
					}
				break;
			}
		case WRITE_SETTINGS_CMD:
			break;
		case PING_CMD:
			{	
				rs232_Send_Data(ping_frame, RESPONSE_STATUS_LEN );
				break;
			}
	}
	
}