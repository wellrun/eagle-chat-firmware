#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#define PROTOCOL_DELIM	":" // The separator character for protocol message sections

// Commands
#define PROTOCOL_TOKEN_SEND				's' // ex. s:[0-9]{1,3}(address):[message string]
#define PROTOCOL_TOKEN_SET_KEY			'p' // ex. p:[0-9]{1,3}(address):[32 bytes]
#define PROTOCOL_TOKEN_GENKEYS			'k' // ex. k
#define PROTOCOL_TOKEN_PUBKEY_UPDATE	'u' // ex. u:[0-9]{1,3}(address)
#define PROTOCOL_TOKEN_SET_ID			'i' // ex. i:[1 byte]

// Get requests
// Format: g:(var_token)
#define PROTOCOL_TOKEN_GET				'g'
#define PROTOCOL_TOKEN_GET_KEY			'p'
#define PROTOCOL_TOKEN_GET_ID			'i'
#define PROTOCOL_TOKEN_GET_STATUS		's'

// Responses to host commands
#define PROTOCOL_RESPONSE_PREFIX		'x'
#define PROTOCOL_RESPONSE_FAIL			"FAIL" 	// ex. x:FAIL
#define PROTOCOL_RESPONSE_OK			"OK"	// ex. x:OK
#define PROTOCOL_RESPONSE_YES			"YES"	// ex. x:YES
#define PROTOCOL_RESPONSE_NO			"NO"	// ex. x:NO



#endif // PROTOCOL_H__
