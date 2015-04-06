#ifndef PACKET_FIFO_H_INCLUDED
#define PACKET_FIFO_H_INCLUDED

#include "asf.h"
#include "routing.h"

#define PACKET_FIFO_UNIT_LEN   (MAX_PAYLOAD_SIZE + 1) // Add one byte store length
#define PACKET_FIFO_SLOTS      4
#define PACKET_FIFO_SIZE       (PACKET_FIFO_SLOTS*PACKET_FIFO_UNIT_LEN)

typedef struct {
	uint8_t data[PACKET_FIFO_SIZE];
	uint16_t h;
	uint16_t t;
    uint8_t stored;
} packet_fifo_t;

//! Initializes a packet_fifo_t structure
/*!
  \param f 		The packet_fifo structure
*/
void packet_fifo_init(packet_fifo_t *f);

//! Checks the contents of the packet_fifo. Returns 1 if empty, 0 if not empty
/*!
  \param f 		The packet_fifo structure
*/
uint8_t packet_fifo_isEmpty(packet_fifo_t *f);

uint8_t packet_fifo_isFull(packet_fifo_t *f);

//! Writes one PACKET_FIFO_UNIT_LEN slot of the packet_fifo
/*!
  \param f 		       The packet_fifo structure
  \param h  	       PacketHeader describing the packet
  \param payload       Data to write into fifo payload section.
  \param payloadLen    Length of payload. Cannot be greater than MAX_PACKET_PAYLOAD_SIZE
*/
void packet_fifo_write(packet_fifo_t *f, PacketHeader h, uint8_t *payload, uint8_t payloadLen);

/*! Exposes packet structures stored in fifo

    NOTE: Once this function is called, the data could be overwritten if another
    packet is written into the fifo. You must copy the data to prevent corruption
    if it not used before storing more packets.
*/
/*!
  \param f          The packet_fifo structure
  \parah h          Pointer to PacketHeader, written by function
  \param payload    Pointer to hold address of payload buffer
  \return           Length of payload, payloadLen
*/
uint8_t packet_fifo_read(packet_fifo_t *f, PacketHeader *h, uint8_t **payload);

PacketHeader packet_fifo_peekHeader(packet_fifo_t *f);

//! Returns the byte at offset in the current tail
/*!
  \param f 		The packet_fifo structure
  \param offset	The byte offset to read
*/
//uint8_t packet_fifo_peek(packet_fifo_t *f, uint16_t offset);

//! Moves tail forward one slot
/*!
  \param f 		The packet_fifo structure
*/
void packet_fifo_skip(packet_fifo_t *f);


#endif /* PACKET_FIFO_H_INCLUDED */
