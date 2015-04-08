#ifndef ROUTING_H_INCLUDED
#define ROUTING_H_INCLUDED

#include "asf.h"
#include "radio.h"

#define PACKET_HEADER_SIZE      	(sizeof(PacketHeader))
#define MAX_ACK_FAILURES        	10

typedef struct {
	uint8_t source;
	uint8_t dest;
	uint8_t type;
} __attribute__((packed)) PacketHeader;


#define PACKET_TYPE_CONTENT     	0
#define PACKET_TYPE_RRQ         	1
#define PACKET_TYPE_RUP         	2
#define PACKET_TYPE_FAIL			3


#define RRQ_PACKET_HEADER_SIZE  	(sizeof(RRQPacketHeader))

typedef struct {
	uint8_t rrqID;
	uint8_t hopcount;
} __attribute__((packed)) RRQPacketHeader;

#define CONTENT_PACKET_HEADER_SIZE	(sizeof(ContentPacketHeader))

typedef struct {
	uint16_t contentId;
	uint8_t encrypted;
} __attribute__((packed)) ContentPacketHeader;

#define MAX_PACKET_PAYLOAD_SIZE 	(MAX_PAYLOAD_SIZE - PACKET_HEADER_SIZE)     // currently: 246
#define MAX_CONTENT_PAYLOAD_SIZE	(MAX_PAYLOAD_SIZE - PACKET_HEADER_SIZE - CONTENT_PACKET_HEADER_SIZE)

typedef struct {
	uint8_t nextHop;
	uint8_t failures;
	uint16_t originalRRQID;
} RoutingTableEntry;

typedef struct {
	uint8_t  dest;
	uint32_t timestamp;
	bool active;
	bool resolved;
	bool timeout;
} RRQProgress;

#define ROUTING_TABLE_ENTRIES   256

//! Configures the routing module and its dependencies
void setupRouting(uint8_t nodeId);

//! Consults the routing table and attempts to send a packet
//! It is assumed that every packet going out WILL have a PacketHeader, so one is taken in the argument
//! The client is responsible for packing any other header needed into the payload themselves
bool sendPacket(PacketHeader *h, uint8_t *payload, uint8_t payloadLen);

//! Processes received packets, sends ACKS, forwards packets, etc.
void handleReceived(void);

void initiateRouteRequest(uint8_t dest);

void debugPrintRoutingTable(uint8_t r);
void debugPrintRecentRRQ(void);
void debugPrintRRQProgress(void);


bool routeExistsTo(uint8_t dest);
bool routeRequestInProgress(void);

bool queuePacket(PacketHeader h, uint8_t *payload, uint8_t payloadLen);
void processSendQueue(void);

bool packetsToRead(void);

uint8_t packetReceivedPeek(PacketHeader *h, uint8_t **payload);

void packetReceivedSkip(void);



RRQProgress *getRrqProgress(void);

#endif /*ROUTING_H_INCLUDED*/
