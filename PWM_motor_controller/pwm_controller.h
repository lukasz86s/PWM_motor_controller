/*
 * pwm_controller.h
 *
 * Created: 24.03.2025 09:39:18
 *  Author: surmi
 */ 


#ifndef PWM_CONTROLLER_H_
#define PWM_CONTROLLER_H_
#include <avr/io.h>

typedef enum {
	PWM_CHANNEL_1,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4,
	PWM_CHANNEL_5,
	PWM_CHANNEL_6,
	NUMBER_OF_CHANNELS	
} PWM_Channel_t;

typedef enum {
	PWM_PINOUT_1 = (1<<0),
	PWM_PINOUT_2 = (1<<1),
	PWM_PINOUT_3 = (1<<2),
	PWM_PINOUT_4 = (1<<3),
	PWM_PINOUT_5 = (1<<4),
	PWM_PINOUT_6 = (1<<5)
} PWM_Pins_t;

typedef enum{
	PWM_OUTPUT_MODE_0,
	PWM_OUTPUT_MODE_1,
	PWM_OUTPUT_MODE_2,
	PWM_OUTPUT_MODE_3,
	PWM_OUTPUT_MODE_4, 
	PWM_OUTPUT_MODE_5, 
	PWM_OUTPUT_MODE_6
	}PWM_Output_Mode_t;

void pwm_Init(void);
void pwm_Change_Output_Mode(PWM_Output_Mode_t mode);
void pwm_Set_Duty(PWM_Channel_t channel, uint8_t value);
uint8_t pwm_Get_Duty(uint8_t channel);
uint32_t timer_get_time_ms(void);




#endif /* PWM_CONTROLLER_H_ */