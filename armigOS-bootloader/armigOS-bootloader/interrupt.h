#ifndef _INTERRUPT_H
#define _INTERRUPT_H

// Initialize the bootloader's interrupt vector
void interrupt_init();

// Allow remote to proceed sending characters
void uart_enable_flow();

#endif