#ifndef __STACK_H
#define __STACK_H

#include <stdint.h>

void stack_paint(void) __attribute__ ((naked)) __attribute__ ((section(".init1")));

uint16_t stack_count(void);

#endif // __STACK_H
