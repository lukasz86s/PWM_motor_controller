/*
 * command_parser.h
 *
 * Created: 26.03.2025 10:14:50
 *  Author: surmi
 */ 


#ifndef COMMAND_PARSER_H_
#define COMMAND_PARSER_H_
typedef enum{
	RESERVED_CMD = 0,
	WRITE_CMD,
	READ_CMD,
	WRITE_MANY_CMD,
	WRITE_SETTINGS_CMD
	}CMD_byte_t;

typedef enum{
	NO_ERROR = 0,
	EMPTY_BUFFER_ERROR,
	CRC16_CHECK_ERROR,
	DATA_LEN_ERROR
}Parser_error_t;

void Refresh_Channel_Settings(void);
#endif /* COMMAND_PARSER_H_ */