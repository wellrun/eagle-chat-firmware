#include "msgbuf.h"
#include "asf.h"

static uint8_t head_page, tail_page;
static uint8_t filled = 0;

void msgbuf_init(void)
{
    head_page = tail_page = filled = 0;
}


/* Returns the length of the next message if there is an unconsumed message in the buffer
    return - Length of next message, 0 if no message
*/
uint8_t msgbuf_next_length(void)
{
    if (filled > 0) {
        uint16_t address = MSGBUF_PAGE_SIZE * tail_page;
        irqflags_t flags = cpu_irq_save(); // make this operation atomic
        uint8_t len = sram_read_byte(address);
        cpu_irq_restore(flags);
        return len;
    }
    else
        return 0;
}

/* Stores a message in SRAM
    param msg - Pointer to array holding message contents
    param len - Length of message to store
    return - true if successfully, false if buffer is full
*/
bool msgbuf_store_message(uint8_t * msg, uint8_t len)
{
    if (filled == MSGBUF_MAX_PAGES) {
        return false; // the buffer is full
    }
    uint16_t address = MSGBUF_PAGE_SIZE * head_page;
    irqflags_t flags = cpu_irq_save(); // make this operation atomic
    sram_set_mode(SRAM_MODE_SEQUENTIAL);
    sram_write_byte(address, len);
    sram_write_packet(address + 1, msg, len);
    cpu_irq_restore(flags);
    ++filled;
    head_page = (head_page + 1) % MSGBUF_MAX_PAGES;

    return true;
}

/* Reads a message from SRAM into memory
    param dest - Pointer to buffer to hold message. Must be long enough to hold entire message
    return - true if successful read message, false if no message was available
*/
bool msgbuf_read_message(uint8_t * dest)
{
    if (filled == 0)
        return false;

    uint16_t address = MSGBUF_PAGE_SIZE * tail_page;
    irqflags_t flags = cpu_irq_save(); // make this operation atomic
    sram_set_mode(SRAM_MODE_SEQUENTIAL);
    uint8_t len = sram_read_byte(address);
    sram_read_packet(address + 1, dest, len);
    cpu_irq_restore(flags);

    --filled;
    tail_page = (tail_page + 1) % MSGBUF_MAX_PAGES;

    return true;
}
