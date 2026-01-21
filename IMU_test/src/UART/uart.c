#include <UART/uart.h>
#include "stdio.h"
#include <string.h>
/* 初始化指定串口 */
void uart_init(uart_port_t port)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (port)
    {
    case UART_PORT_4:
        err = R_SCI_UART_Open(&g_uart4_ctrl, &g_uart4_cfg);
        assert(FSP_SUCCESS == err);
        break;
    default:
        /* 未配置的串口：不做处理（可扩展其他 g_uartX） */
        break;
    }
}

/* 发送完成标志 */
volatile bool uart_send_complete_flag = false;

/* 串口中断回调 */
void g_uart4_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
    case UART_EVENT_RX_CHAR:
    {

        break;
    }
    case UART_EVENT_TX_COMPLETE:
    {
        uart_send_complete_flag = true;
        break;
    }
    default:
        break;
    }
}
//串口与上位机通讯
void uart_printf(uart_port_t uart_num, const char *str)
{
    if (str == NULL)
    {
        return;
    }
    const char *p = str;
    while (*p)
    {
        uint8_t out_ch = (uint8_t)*p;
        p++;

        switch (uart_num)
        {
        case 4:
        default:
        {
            uart_send_complete_flag = false;
            (void)R_SCI_UART_Write(&g_uart4_ctrl, &out_ch, 1);
            while (uart_send_complete_flag == false)
                ;
            uart_send_complete_flag = false;
            break;
        }
        }
    }
}
