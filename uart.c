#define F_CPU 16000000
#define SPEED 9600

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

void uart_init(void)
{
	unsigned int bittimer;

	bittimer = (F_CPU / SPEED / 16) - 1;

	/* Set the baud rate */
	UBRR0H = (unsigned char)(bittimer >> 8);
	UBRR0L = (unsigned char)bittimer;

	/* set the framing to 8N1 */
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	/* Enable Rx & Tx */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

int uart_putchar(char c, FILE * stream)
{
	if (c == '\a') {
		fputs("*ring*\n", stderr);
		return 0;
	}

	if (c == '\n')
		uart_putchar('\r', stream);
	while (!(UCSR0A & (1 << UDRE0))) ;
	UDR0 = c;

	return 0;
}
