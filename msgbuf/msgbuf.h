#ifndef MSGBUG_H__
#define MSGBUG_H__

/****
    Implements a circular buffer stored in external SRAM
    Contents are stored in packets or pages of a fixed size

    SRAM store and load operations are atomic, however the buffer's tracking
    variables are not. Do not use this module from both an interrupt routine
    and the main thread.
****/

#include "sram/sram.h"

#define MSGBUF_MAX_ADDRESS  0x0EFF  /* Highest memory address we can use */
#define MSGBUF_PAGE_SIZE    256     /* Page size in bytes */
#define MSGBUF_MAX_PAGES    16      /* Max number of pages we can store */

/*
Page memory layout:
[len (byte)] | [message (0 ... MSGBUF_PAGE_SIZE - 1)]
*/

void msgbuf_init(void);

/* Returns the length of the next message if there is an unconsumed message in the buffer
    return - Length of next message, 0 if no message
*/
uint8_t msgbuf_next_length(void);

/* Stores a message in SRAM
    param msg - Pointer to array holding message contents
    param len - Length of message to store
    return - true if successfully, false if buffer is full
*/
bool msgbuf_store_message(uint8_t * msg, uint8_t len);

/* Reads a message from SRAM into memory
    param dest - Pointer to buffer to hold message. Must be long enough to hold entire message
    return - true if successful read message, false if no message was available
*/
bool msgbuf_read_message(uint8_t * dest);


#endif
