/**
 * \file
 *
 * \brief Application CDC Definitions
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef CDC_H_INCLUDED
#define CDC_H_INCLUDED

#include "udc.h"
#include "udi_cdc.h"

//! \brief Serial number to be passed to USB stack
extern uint8_t cdc_serial_number[];

//! \brief Start up the needed parts for USB CDC
void cdc_start(void);

/**
 * \brief Called by CDC interface to open/close port communication
 *
 * \param enable Open if true, and close if false
 */
void cdc_set_dtr(bool enable);

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host enable CDC interface
 *
 * \retval true if cdc startup is successfully done
 */
bool cdc_enable(void);

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host disable cdc interface
 */
void cdc_disable(void);

/**
 * \brief Called by CDC interface
 *
 * Callback when data has been received
 */
void cdc_rx_notify(uint8_t);

/**
 * Prints a null terminated string over USB
 */
void cdc_write_string(const uint8_t* buf);

/**
 * Prints a byte as its human-readable HEX form over USB
 */
void cdc_write_hex(const uint8_t c);

/**
 * Prints a newline character
 */
void cdc_newline();

/**
 * Prints a message followed by a string representation of 'value',
 * and finishes with a newline.
 * E.g. cdc_log_int("The value is: ", 5);
 */
void cdc_log_int(uint8_t *message, uint32_t value);

/**
 * Reads USB input into 'buffer' until a newline character
 * is encountered or 'maxlen' is reached
 */
void cdc_read_string(uint8_t *buffer, uint32_t maxlen);

#endif /* CDC_H_INCLUDED */
