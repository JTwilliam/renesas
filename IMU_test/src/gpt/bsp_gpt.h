/*
 * bsp_gpt.h
 *
 *  Created on: 2026年1月21日
 *      Author: huago
 */

#ifndef GPT_BSP_GPT_H_
#define GPT_BSP_GPT_H_

#include "hal_data.h"

extern uint32_t pwm_period;
extern uint32_t pwm_freq;
extern uint32_t pwm_duty;
extern uint32_t pwm_high_level;

extern timer_info_t gpt2_info;

void Gpt_Init(void);
void Gpt1_Pwm_Setduty(uint8_t duty);
void Gpt2_Pwm_Setduty(uint8_t duty);


#endif /* GPT_BSP_GPT_H_ */
