#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include "asf.h"

#define FIFO_UNIT_LEN   256
#define FIFO_SLOTS      4
#define FIFO_SIZE       (FIFO_SLOTS*FIFO_UNIT_LEN)

typedef struct {
	uint8_t data[FIFO_SIZE];
	uint32_t h;
	uint32_t t;
} fifo_t;

//! Initializes a fifo_t structure
/*!
  \param f 		The fifo structure
*/
void fifo_init(fifo_t *f);

//! Checks the contents of the fifo. Returns 1 if empty, 0 if not empty
/*!
  \param f 		The fifo structure
*/
uint8_t fifo_isEmpty(fifo_t *f);

//! Writes one FIFO_UNIT_LEN slot of the fifo
/*!
  \param f 		The fifo structure
  \param buf	The source buffer
  \param len 	The amount of data from buf to write. Note that the write action still uses up a whole FIFO_UNIT_LEN slot of the fifo
*/
void fifo_write(fifo_t *f, uint8_t *buf, uint8_t len);

//! Returns a pointer to the next FIFO_UNIT_LEN bytes of data
/*!
  \param f 		The fifo structure
*/
uint8_t * fifo_read(fifo_t *f);

//! Returns the byte at offset in the current tail
/*!
  \param f 		The fifo structure
  \param offset	The byte offset to read
*/
uint8_t fifo_peek(fifo_t *f, uint16_t offset);

//! Moves tail forward one slot
/*!
  \param f 		The fifo structure
*/
void fifo_skip(fifo_t *f);


#endif /* FIFO_H_INCLUDED */