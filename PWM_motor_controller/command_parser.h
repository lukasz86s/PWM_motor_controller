/*
 * command_parser.h
 *
 * Created: 26.03.2025 10:14:50
 *  Author: surmi
 */ 


#ifndef COMMAND_PARSER_H_
#define COMMAND_PARSER_H_
typedef enum{
	RESERVED_CMD,
	WRITE_CMD,
	READ_CMD,
	WRITE_MANY_CMD,
	WRITE_SETTINGS_CMD
	}CMD_byte_t;

void Refresh_Channel_Settings(void);
#endif /* COMMAND_PARSER_H_ */