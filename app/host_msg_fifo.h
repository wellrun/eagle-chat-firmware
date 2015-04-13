#ifndef HOST_MSG_FIFO__
#define HOST_MSG_FIFO__

#include <asf.h>
#include <stdbool.h>

#define HOST_MSG_BUF_SIZE        200

typedef struct {
	uint8_t len; // the number of non-null bytes in this message
	uint8_t data[HOST_MSG_BUF_SIZE];
} hostMsg_t;

#define HOST_MSG_FIFO_UNIT_LEN   (sizeof(hostMsg_t))
#define HOST_MSG_FIFO_SLOTS      4
#define HOST_MSG_FIFO_SIZE       (HOST_MSG_FIFO_SLOTS*HOST_MSG_FIFO_UNIT_LEN)

typedef struct {
	uint8_t data[HOST_MSG_FIFO_SIZE];
	uint16_t h;
	uint16_t t;
	uint8_t stored;
} hostMsg_fifo_t;

//! Initializes a hostMsg_fifo_t structure
/*!
  \param f 		The packet_fifo structure
*/
void hostMsg_fifo_init(hostMsg_fifo_t *f);

//! Checks the contents of the hostMsg_fifo. Returns 1 if empty, 0 if not empty
/*!
  \param f 		The hostMsg_fifo structure
*/
bool hostMsg_fifo_isEmpty(hostMsg_fifo_t *f);

bool hostMsg_fifo_isFull(hostMsg_fifo_t *f);

//! Writes one HOST_MSG_FIFO_UNIT_LEN slot of the hostMsg_fifo
/*!
  \param f 			   The hostMsg_fifo structure
  \param msg  		   PacketHeader describing the packet
*/
void hostMsg_fifo_write(hostMsg_fifo_t *f, hostMsg_t * msg);

/*! Expose the hostMsg_t at tail

	NOTE: Once this function is called, the data could be overwritten if another
	packet is written into the fifo. You must copy the data to prevent corruption
	if it not used before storing more messages.
*/
/*!
  \param f      The hostMsg_fifo structure
  \return       Pointer to hostMsg_t in fifo
*/
hostMsg_t * hostMsg_fifo_read(hostMsg_fifo_t *f);

//! Expose the hostMsg_t at tail without moving the tail
/*!
  \param f      The hostMsg_fifo structure
  \return       Pointer to hostMsg_t in fifo
*/
hostMsg_t * hostMsg_fifo_peek(hostMsg_fifo_t *f);

//! Moves tail forward one slot
/*!
  \param f 		The hostMsg_fifo structure
*/
void hostMsg_fifo_skip(hostMsg_fifo_t *f);

//! Copies the string buf into the msg, updating len
void hostMsg_addString(hostMsg_t *msg, uint8_t *buf);

void hostMsg_addByte(hostMsg_t *msg, uint8_t byte);

void hostMsg_addBuffer(hostMsg_t *msg, uint8_t *buf, uint8_t len);

void hostMsg_addInt(hostMsg_t *msg, uint32_t num, uint8_t base);

void hostMsg_addHex(hostMsg_t *msg, uint8_t byte);

void hostMsg_addHexBuffer(hostMsg_t *msg, uint8_t *buf, uint8_t len);

uint8_t hostMsg_hexToByte(uint8_t high, uint8_t low);

void hostMsg_hexBufferToBytes(uint8_t *hexbuf, uint8_t len, uint8_t *bytesbuf);

void hostMsg_terminate(hostMsg_t *msg);

uint8_t scaleByteFromHex(uint8_t byte);

#endif // HOST_MSG_FIFO__
