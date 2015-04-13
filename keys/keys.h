#ifndef KEYS_H__
#define KEYS_H__

#include "asf.h"
#include "crypto/avrnacl.h"

#define PAGE_SIZE           32
#define MAX_PAGES           64
#define MAX_BYTES           PAGE_SIZE * MAX_PAGES /* 2048 */
#define MAX_KEY_SLOTS       16

#define PAGE_STATUS         0
#define PAGE_PRIVATE_KEY    1
#define PAGE_PUBLIC_KEY     2
#define PAGE_TABLE          3 /* Page that holds the user key look-up table */
#define PAGE_KEY_START      4

#define FLAGS_UNSET         0xFF // The status register looks like this when it is unconfigured
#define FLAGS_CONFIGURED    0b10000000
#define FLAGS_PRIVATE_KEY   0b01000000
#define FLAGS_PUBLIC_KEY    0b00100000
#define FLAGS_NETWORK_ID    0b00010000
#define FLAGS_PASSWORD      0b00001000

#define KEYS_BIT_SET(val, mask)     !(val & mask) // provides inverted view of set bits
#define KEYS_SET_BIT(val, mask)     (val & ~mask)
#define KEYS_CLEAR_BIT(val, mask)   (val | mask)

typedef struct {
	uint8_t flags;
	uint8_t node_id;
	uint8_t password[16];
} setup_status_t;

// Status storage

//! Loads the setup status from EEPROM
uint8_t load_setup_status(void);

//! Returns the pointer to the setup status
const setup_status_t * get_setup_status(void);

//! Commits the setup status to the EEPROM
uint8_t store_setup_status(void);


// Flags storage

//! Sets a particular flag
void set_status_flag(uint8_t mask);

//! Clears a particular flag
void unset_status_flag(uint8_t mask);

// Keys storage:

uint8_t load_private_key(void);
uint8_t * get_private_key(void);
uint8_t store_private_key(uint8_t key[PAGE_SIZE]);

uint8_t load_public_key(void);
uint8_t * get_public_key(void);
uint8_t store_public_key(uint8_t key[PAGE_SIZE]);


typedef struct {
	uint8_t node_id;
	uint8_t key_page;
} key_table_entry_t;

typedef struct {
	key_table_entry_t table[MAX_KEY_SLOTS];
} key_table_t;


// Shared secret storage

//! Read the ssk storage table from EEPROM into the device
uint8_t ssk_load_table(void);

//! Reset the table
uint8_t ssk_reset_table(void);

//! Retrieve reference to the table while it's in device memory
const key_table_t * ssk_get_table(void);

//! Returns true if there is a table entry for the network id
bool ssk_has_key(uint8_t node_id, uint8_t * slot);

//! Stores the ssk for a certain node
uint8_t ssk_store_key(uint8_t node_id, uint8_t key[PAGE_SIZE]);

//! Retreives an ssk for a certain node
uint8_t ssk_read_key(uint8_t slot, uint8_t dest[PAGE_SIZE]);

//! Commit the ssk storage table back to the EEPROM
uint8_t ssk_store_table(void);


#endif
