/*
 * command_parser.c
 *
 * Created: 26.03.2025 10:14:23
 *  Author: surmi
 */ 
#include "command_parser.h"
#include "pwm_controller.h"
#include "rs232_communication.h"

#ifndef MAX_PWM_CHANNELS
	#define MAX_PWM_CHANNELS 10
#endif
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
	
uint8_t parse_frame(void)
{
	uint8_t* data = rs232_Get_Frame();
	if(data == 0) return 1;
	uint8_t len = data[FRAME_LEN];
	len +=1;
	// TODO: implement function check_crc(len, data)
	PWM_Channel_Cofnig.function = data[CMD];
	PWM_Channel_Cofnig.channels_to_set = data[CHANNELS_TO_SET];
	// TODO: add channel data amount calculation (len - nr_of_channels*2 == x)
	for(uint8_t channel_config_idx = 0; channel_config_idx < PWM_Channel_Cofnig.channels_to_set; channel_config_idx++)
	{	
		PWM_Channel_Cofnig.channel_number[channel_config_idx]  = data[FIRST_CHANNELS_DATA + 2*channel_config_idx];
		PWM_Channel_Cofnig.channel_value[channel_config_idx ] = data[FIRST_CHANNELS_DATA + 2*channel_config_idx +1];
	}
	return 0;
	
}
/*--------------futciotn only to test.correctness of transmitted data-----------*/
void print_pwm_stat(Frame_Fields * stats)
{
	rs232_Send_Data(&stats->function, 1);
	rs232_Send_Data(&stats->channels_to_set, 1);
	rs232_Send_Data(stats->channel_number, stats->channels_to_set);
	rs232_Send_Data(stats->channel_value, stats->channels_to_set);
	
}
/*-------------------------------------------------------------------------*/
	void Refresh_Channel_Settings(void)
{
	uint8_t status = parse_frame();
	if(status == 1)
	{	
		// TODO: implement 'ping' function 
		uint8_t test_data[] ={95, 96, 97, 98, 99};
		rs232_Send_Data(test_data,4);
	}else
	{
		print_pwm_stat(&PWM_Channel_Cofnig);
		switch(PWM_Channel_Cofnig.function){
			case WRITE_CMD:
				pwm_Set_Duty(PWM_Channel_Cofnig.channel_number[0], PWM_Channel_Cofnig.channel_value[0]);
				break;
			case READ_CMD:
				break;
			case WRITE_MANY_CMD:
				for(uint8_t i=0; i<PWM_Channel_Cofnig.channels_to_set; i++)
					{
						pwm_Set_Duty(PWM_Channel_Cofnig.channel_number[i], PWM_Channel_Cofnig.channel_value[i] );
					}
				break;
			case WRITE_SETTINGS_CMD:
				break;
		}
	}
	
}