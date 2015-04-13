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

typedef struct {
	uint8_t node_id;
	uint8_t key_page;
} key_table_entry_t;

typedef struct {
	key_table_entry_t table[MAX_KEY_SLOTS];
} key_table_t;


//! Load functions - read the EEPROM and set the in-memory data structures
uint8_t load_private_key(void);
uint8_t load_public_key(void);
uint8_t load_setup_status(void);

uint8_t ssk_load_table(void);


//! Get functions - return pointers to the data structures in-memory, or retrieve information from them
uint8_t * get_private_key(void);
uint8_t * get_public_key(void);
const setup_status_t * get_setup_status(void);

const key_table_t * ssk_get_table(void);
bool ssk_has_key(uint8_t node_id, uint8_t * slot);
uint8_t ssk_read_key(uint8_t slot, uint8_t dest[PAGE_SIZE]);


//! Set functions - Set information on the data structures in-memory (which is automatically backed by the EEPROM)
uint8_t set_private_key(uint8_t key[PAGE_SIZE]);
uint8_t set_public_key(uint8_t key[PAGE_SIZE]);
void set_status_flag(uint8_t mask);
void unset_status_flag(uint8_t mask);

uint8_t ssk_set_key(uint8_t node_id, uint8_t key[PAGE_SIZE]);
uint8_t ssk_reset_table(void);





#endif
