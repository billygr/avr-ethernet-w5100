#ifndef UART_H
#define UART_H

void uart_init(void);
int uart_putchar(char c, FILE * stream);
int uart_getchar(FILE * stream);
unsigned char uart_kbhit(void);

#endif
