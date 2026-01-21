/*
 * bsp_gpt.h
 *
 *  Created on: 2026年1月21日
 *      Author: huago
 */

#ifndef GPT_BSP_GPT_H_
#define GPT_BSP_GPT_H_

#include "hal_data.h"

void Gpt_Init(void);
void Gpt_Pwm_Setduty(uint8_t duty);

#endif /* GPT_BSP_GPT_H_ */
