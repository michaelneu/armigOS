#ifndef _UART_H
#define _UART_H

#include <stdbool.h>

// 1 Baud = 1 symbol/second
typedef enum { 
	BAUD_2400 = 416,
	BAUD_4800 = 207,
	BAUD_9600 = 103
} BAUD_RATE_T;

typedef enum {
	FLOW_XON = 17,
	FLOW_XOFF = 19
} UART_FLOW_CONTROL_T;

void uart_init(BAUD_RATE_T baud_rate);

bool uart_can_send();

void uart_send(unsigned char character);
void uart_send_string(char *string);

unsigned char uart_receive();
unsigned char uart_receive_blocking();
unsigned char uart_receive_echo();

void uart_receive_string(char *buffer, int buffer_length);
void uart_receive_typed_string(char *buffer, int buffer_length);
void uart_read_line(char *buffer, int buffer_length);

void uart_clear_terminal();

#endif