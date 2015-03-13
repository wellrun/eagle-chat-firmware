#include "packetbuf.h"
#include "asf.h"
#include "cdc.h"

static uint8_t head_page, tail_page;
static uint8_t filled = 0;

void packetbuf_init(void)
{
    head_page = tail_page = filled = 0;
}


/* Returns the length of the next message if there is an unconsumed message in the buffer
    return - Length of next message, 0 if no message
*/
bool packetbuf_has_next(void)
{
    return filled > 0;
}

/* Stores a message in SRAM
    param msg - Pointer to array holding message contents
    param len - Length of message to store
    return - true if successfully, false if buffer is full
*/
bool packetbuf_store_packet(uint8_t * msg, uint8_t len)
{
    if (filled == PACKETBUF_MAX_PAGES) {
        return false; // the buffer is full
    }
    uint16_t address = PACKETBUF_PAGE_SIZE * head_page + PACKETBUF_MIN_ADDRESS;
    irqflags_t flags = cpu_irq_save(); // make this operation atomic
    sram_set_mode(SRAM_MODE_SEQUENTIAL);
    sram_write_packet(address, msg, len);
    cpu_irq_restore(flags);
    ++filled;
    head_page = (head_page + 1) % PACKETBUF_MAX_PAGES;

    return true;
}

/* Reads a message from SRAM into memory
    param dest - Pointer to buffer to hold message. Must be long enough to hold entire message
    return - true if successful read message, false if no message was available
*/
bool packetbuf_read_packet(uint8_t * dest)
{
    if (filled == 0)
        return false;

    uint16_t address = PACKETBUF_PAGE_SIZE * tail_page + PACKETBUF_MIN_ADDRESS;
    irqflags_t flags = cpu_irq_save(); // make this operation atomic
    sram_set_mode(SRAM_MODE_SEQUENTIAL);
    sram_read_packet(address, dest, PACKETBUF_PAGE_SIZE);
    cpu_irq_restore(flags);

    --filled;
    tail_page = (tail_page + 1) % PACKETBUF_MAX_PAGES;

    return true;
}
