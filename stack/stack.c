#include "stack.h"

#define STACK_CANARY    0xc5

extern uint8_t _end;
extern uint8_t __stack;

void stack_paint(void)
{
#if 0
	uint8_t *p = &_end;

	while (p <= &__stack) {
		*p = STACK_CANARY;
		p++;
	}
#else
	__asm volatile ("    ldi r30,lo8(_end)\n"
					"    ldi r31,hi8(_end)\n"
					"    ldi r24,lo8(0xc5)\n" /* STACK_CANARY = 0xc5 */
					"    ldi r25,hi8(__stack)\n"
					"    rjmp .cmp\n"
					".loop:\n"
					"    st Z+,r24\n"
					".cmp:\n"
					"    cpi r30,lo8(__stack)\n"
					"    cpc r31,r25\n"
					"    brlo .loop\n"
					"    breq .loop" ::);
#endif
}

uint16_t stack_count(void)
{
    const uint8_t *p = &_end;
    uint16_t       c = 0;

    while(*p == STACK_CANARY && p <= &__stack)
    {
        p++;
        c++;
    }

    return c;
}
