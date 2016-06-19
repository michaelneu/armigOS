#include "uart.h"
#include "ringbuffer.h"
#include <stdbool.h>
#include <avr/interrupt.h>

volatile bool FLOW_XOFF_SENT = false;

// usart interrupt -> append new characters to ringbuffer
ISR(USART_RX_vect)
{
	if (ringbuffer_almost_full() && !FLOW_XOFF_SENT)
	{
		uart_send(FLOW_XOFF);
		
		FLOW_XOFF_SENT = true;
	}
	
	ringbuffer_append(uart_receive_echo());
}

void uart_enable_flow()
{
	if (FLOW_XOFF_SENT)
	{
		uart_send(FLOW_XON);
		FLOW_XOFF_SENT = false;
	}
}

void interrupt_init()
{
	uint8_t temp = MCUCR;
	
	MCUCR = temp | (1 << IVCE);
	MCUCR = temp | (1 << IVSEL);
}