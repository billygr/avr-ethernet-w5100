#define SPEED 9600

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

/* For ATMEga168 ONLY. you need to adapt it to other AVR CPUs   */

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

int uart_getchar(FILE * stream)
{
	unsigned char ch;
	while (!(UCSR0A & (1 << RXC0))) ;
	ch = UDR0;

	/* Echo the Output Back to terminal */
	uart_putchar(ch, stream);

	return ch;
}

unsigned char uart_kbhit(void)
{
	//return nonzero if char is waiting (polled version)
	unsigned char b;
	b = 0;
	if (UCSR0A & (1 << RXC0))
		b = 1;
	return b;
}
