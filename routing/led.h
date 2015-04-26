#ifndef LED_H__
#define LED_H__ 

#define LED_TIMER 			TCD0

#define LED_TIMER_COUNT 	15625
#define LED_TIMER_DIV 		TC_CLKSEL_DIV1024_gc


void led_init(void);

void led_on(void);


#endif