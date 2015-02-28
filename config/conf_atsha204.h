#ifndef CONF_ATSHA204_H_INCLUDED
#define CONF_ATSHA204_H_INCLUDED

#include <board.h>
#include "user_board.h"

#define ATSHA204_TWI_PORT 			&TWI_ATSHA

// Board independent configuration
#define ATSHA204_TWI_SPEED          (400000)

//! TWI address used at SHA204 library startup
#define SHA204_I2C_DEFAULT_ADDRESS  (0xCA)

#endif /* CONF_ATSHA204_H_INCLUDED */