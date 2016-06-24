#ifndef _INTERRUPT_H
#define _INTERRUPT_H

// Initialize the bootloader's interrupt vector
void interrupt_init();

// Reset the interrupt vector back to 0x0002
void interrupt_reset();

// Allow remote to proceed sending characters
void uart_enable_flow();

#endif