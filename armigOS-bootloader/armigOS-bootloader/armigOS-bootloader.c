#include "interrupt.h"
#include "parser.h"
#include "ringbuffer.h"
#include "uart.h"
#include "utils.h"

void program_device()
{
	uart_send_string("\npaste an Intel hex file below: \n");
}

int main(void)
{
	int i;
	
	interrupt_init();
	uart_init(BAUD_9600);
	
	uart_send(FLOW_XON);
	uart_clear_terminal();
	
	// await programming input
	for (i = 0; i < 50; i++)
	{
		delay_ms(100);
		
		if (ringbuffer_can_be_read() && ringbuffer_get_char() == 'p')
		{
			program_device();
			
			break;
		}
	}
	
	// start flashed program
	void (*start)(void) = 0x0;
	start();
	
	return 0;
}