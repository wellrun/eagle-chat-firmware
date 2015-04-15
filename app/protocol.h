#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#include "asf.h"
#include "host_msg_fifo.h"

#define PROTOCOL_DELIM					":" // The separator character for protocol message sections
#define PROTOCOL_DELIM_CHAR				':'

// Commands
#define PROTOCOL_TOKEN_SEND				's' 	// ex. s:[0-9]{1,3}(address):[message string]
#define PROTOCOL_TOKEN_SET_KEY			'p' 	// ex. p:[0-9]{1,3}(address):[64 bytes hex]
#define PROTOCOL_TOKEN_GENKEYS			'k' 	// ex. k
#define PROTOCOL_TOKEN_PUBKEY_UPDATE	'u' 	// ex. u:[0-9]{1,3}(address)
#define PROTOCOL_TOKEN_SET_ID			'i' 	// ex. i:[2 bytes hex]
#define PROTOCOL_TOKEN_SET_PASSWORD		'h'		// ex. h:[32 bytes hex]
#define PROTOCOL_TOKEN_AUTH				'a'		// ex. a:[60 bytes hex]
#define PROTOCOL_TOKEN_COMMIT_CONFIG	'c'		// 

#define PROTOCOL_STRING_BURN			"BURN"	

// Get requests
// Format: g:(var_token)
#define PROTOCOL_TOKEN_GET				'g' 
#define PROTOCOL_TOKEN_GET_KEY			'p' 	// g:p
#define PROTOCOL_TOKEN_GET_ID			'i' 	// g:i
#define PROTOCOL_TOKEN_GET_STATUS		's' 	// g:s

// Responses to host commands
#define PROTOCOL_RESPONSE_PREFIX		'x'
#define PROTOCOL_RESPONSE_FAIL			"FAIL" 	// ex. x:FAIL
#define PROTOCOL_RESPONSE_OK			"OK"	// ex. x:OK
#define PROTOCOL_RESPONSE_YES			"YES"	// ex. x:YES
#define PROTOCOL_RESPONSE_NO			"NO"	// ex. x:NO

void protocolReplyOk();
void protocolReplyFail(uint8_t *reason);

void protocolAddReplyPrefix(hostMsg_t *msg);


#endif // PROTOCOL_H__
