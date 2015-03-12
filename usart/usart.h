#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#include "asf.h"

void usart_init(void);

void usart_write_char(uint8_t);
void usart_newline(void);
void usart_write_string(uint8_t *);
void usart_write_line(uint8_t *);

uint8_t usart_read_char(void);

#endif