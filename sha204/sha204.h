#ifndef SHA204_H_INCLUDED
#define SHA204_H_INCLUDED 

#include "conf_atsha204.h"
#include "sha204_comm.h"
#include "sha204_timer.h"
#include "sha204_command_marshaling.h"
#include "sha204_lib_return_codes.h"

#define SHA204_I2C_ADDRESS 200

void sha204_init(void);

//! Returns the device revision number
//  Return the revision number upno success
//  Returns -1 on failure
uint8_t sha204_getDeviceRevision(void);

//! Completely, and irreversably, locks an atsha204 chip.
//  Returns 0 if device was successfully locked.
//  Returns 1 if device was already locked.
uint8_t sha204_fullLock(void);

//! Retrieves 32 random bits
//  Returns 0 on success and -1 on failure.
/*!
  \param destination	The buffer to fill with random bytes - must be length 32
*/
uint8_t sha204_getRandom32(uint8_t *destination);

#endif /* SHA204_H_INCLUDED */