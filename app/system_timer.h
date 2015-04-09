#ifndef __SYSTEM_TIMER_H
#define __SYSTEM_TIMER_H

#include <avr/io.h>
#include <tc.h>

// Configure timer overflow to happen every ~30 ms
#define SYSTEM_TIMER_COUNT  2000
#define SYSTEM_TIMER_DIV    TC_CLKSEL_DIV256_gc
#define SYSTEM_TC           TCC0

/*
* Initialize the system timer module. Always call before using this module.
*/
void system_timer_init(void);

/*
* Set a function to be called on timer interrupt
*/
void system_timer_set_callback(void (*callback)(void));

/*
* Start the system timer
*/
void system_timer_start(void);

#endif // __SYSTEM_TIMER_H
