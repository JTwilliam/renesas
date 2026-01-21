#ifndef _UART_H
#define _UART_H
#include "hal_data.h"
#include "stdio.h"

typedef enum
{
	UART_PORT_1 = 1,
	UART_PORT_2 = 2,
	UART_PORT_3 = 3,
	UART_PORT_4 = 4,
	UART_PORT_5 = 5,
	UART_PORT_6 = 6
} uart_port_t;

void uart_init(uart_port_t port);
void uart_printf(uart_port_t uart_num, const char *str);

#endif
