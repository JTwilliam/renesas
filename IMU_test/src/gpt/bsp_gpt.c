#include "bsp_gpt.h"

timer_info_t gpt2_info;
uint32_t gpt2_period;

void Gpt_Init(void)
{
    R_GPT_Open(&g_timer2_ctrl,&g_timer2_cfg);
    R_GPT_Open(&g_timer1_ctrl,&g_timer1_cfg);
    R_GPT_Open(&g_timer0_ctrl,&g_timer0_cfg);

    R_GPT_Enable(&g_timer2_ctrl);

    R_GPT_Start(&g_timer2_ctrl);
    R_GPT_Start(&g_timer1_ctrl);
    R_GPT_Start(&g_timer0_ctrl);

}

void Gpt_Pwm_Setduty(uint8_t duty)
{
    timer_info_t info;
    uint32_t duty_count;

    if(duty>100)
        duty = 100;

    R_GPT_InfoGet(&g_timer1_ctrl, &info);
    duty_count=(info.period_counts * duty) / 100;
    R_GPT_DutyCycleSet(&g_timer1_ctrl,duty_count, GPT_IO_PIN_GTIOCA);

}


uint32_t pwm_period;
uint32_t pwm_freq;
uint32_t pwm_duty;
uint32_t pwm_high_level;

void gpt2_callback(timer_callback_args_t *p_args)
{
    static uint32_t t;
    static uint32_t t1;
    static uint32_t t2;

    static uint32_t overflow_times;
    static uint32_t one_period_flag = 0;

    switch(p_args->event)
    {

        case TIMER_EVENT_CAPTURE_A:
            if(0 == one_period_flag)
            {
                t = p_args->capture;
                overflow_times = 0;
                one_period_flag ++;
            }
            else if(1 == one_period_flag)
            {
                t2 = p_args->capture + overflow_times * gpt2_period;
                pwm_period = t2 - t;
                overflow_times = 0;
                one_period_flag = 0;
            }
            break;
        case TIMER_EVENT_CAPTURE_B:
            if(1 == one_period_flag)
            {
                t1 = p_args->capture + overflow_times * gpt2_period;
                pwm_high_level = t1 - t;
            }
            break;
        case TIMER_EVENT_CYCLE_END:
            overflow_times ++;
            break;

         default:
             break;
    }

}


void gpt0_callback(timer_callback_args_t *p_args)
{
    R_PORT2->PODR ^= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF);
}
