#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void uart_init(BAUD_RATE_T baud_rate)
{
	// set the baud rate (UBRRn register with n = 0)
	UBRR0H = (unsigned char)(baud_rate >> 8);
	UBRR0L = (unsigned char)baud_rate;
	
	//enable sending and receiving with UCSRnB register
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	/**
	 *                 set data bit size to 0b11 (= 8 bit)
	 *                       in UCSZn0 and UCSZn1
	 *                             |
	 *                             |
	 *  select stop bit count      |
	 * (= 2) in USBSn register     |
	 *            |                |
	 *            v                v
	 */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	// enable all interrupts
	sei();
}

inline bool uart_can_send()
{
	// check whether the "previous content sent"-flag is set
	return UCSR0A & (1 << UDRE0);
}

void uart_send(unsigned char data)
{
	while (!uart_can_send())
	{
		// block until we can send a message
	}
	
	// fix for PuTTY not displaying new lines correctly
	if (data == '\n')
	{
		// send carriage return
		// UDRn = data register n
		UDR0 = '\r';
		
		while (!uart_can_send())
		{
			// wait until we can send again
		}
	}
	
	// send char
	UDR0 = data;
}

void uart_send_string(char *string)
{
	for (int i = 0; string[i] != 0; i++)
	{
		uart_send(string[i]);
	}
}

inline void uart_clear_terminal()
{
	// terminal clear command = 12 (form-feed-character -> new page in terminal)
	uart_send((unsigned char)12);
}

inline bool uart_can_receive()
{
	// check if "content can be received"-flag is set
	return UCSR0A & (1 << RXC0);
}

unsigned char uart_receive()
{
	unsigned char c = UDR0;
	
	// fix for PuTTY: enter sends carriage return -> change it to a newline-character
	if (c == '\r')
	{
		c = '\n';
	}
	
	return c;
}

unsigned char uart_receive_blocking()
{
	while (!uart_can_receive())
	{
		// block until we can receive something
	}
	
	return uart_receive();
}

unsigned char uart_receive_echo()
{
	// receive char
	unsigned char c = uart_receive_blocking();
	
	// and send char back -> char will be displayed
	uart_send(c);

	return c;
}