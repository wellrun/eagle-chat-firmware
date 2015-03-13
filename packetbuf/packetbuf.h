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

#define PACKETBUF_MAX_ADDRESS   0x1DFF  /* Highest memory address we can use */
#define PACKETBUF_MIN_ADDRESS   0x0F00
#define PACKETBUF_PAGE_SIZE     256     /* Page size in bytes */
#define PACKETBUF_MAX_PAGES     15      /* Max number of pages we can store */


/*
Page memory layout:
[message (0 ... PACKETBUF_PAGE_SIZE )]
*/

void packetbuf_init(void);

/* Returns the length of the next message if there is an unconsumed message in the buffer
    return - Length of next message, 0 if no message
*/
bool packetbuf_has_next(void);

/* Stores a message in SRAM
    param msg - Pointer to array holding message contents
    param len - Length of message to store
    return - true if successfully, false if buffer is full
*/
bool packetbuf_store_packet(uint8_t * msg, uint8_t len);

/* Reads a message from SRAM into memory
    param dest - Pointer to buffer to hold message. Must be long enough to hold entire message
    return - true if successful read message, false if no message was available
*/
bool packetbuf_read_packet(uint8_t * dest);


#endif
