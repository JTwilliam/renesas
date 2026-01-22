/*
 * Test_LQZ.c
 *
 *  Created on: 2026年1月22日
 *      Author: huago
 */

#include "Test_LQZ.h"
#include "gpt/bsp_gpt.h"
#include "UART/uart.h"
#include "stdio.h"
#include "inttypes.h"

uint32_t pwm_period = 0;
uint32_t pwm_freq = 0;
uint32_t pwm_duty = 0;
uint32_t pwm_high_level = 0;


void Test_Gpt_Pwm(void)
{
    Gpt_Init();
    uart_init(UART_PORT_4);

    Gpt1_Pwm_Setduty(60);


        while(1)
        {
            //gpt2输入捕获(gpt1输出pwm作gpt2的输入信号)
            if(pwm_period > 0)
            {
                        pwm_freq = gpt2_info.clock_frequency / pwm_period;
                        pwm_duty = pwm_high_level * 100 / pwm_period;
            }
            else
            {
                pwm_freq = 0;
                pwm_duty = 0;
             }


            char buf[64];
            sprintf(buf, "High = %d,Period = %dFreq = %dHZ , Duty = %d%%\r\n", pwm_high_level, pwm_period, pwm_freq, pwm_duty);
            uart_printf(UART_PORT_4, buf);


            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

         }
}



