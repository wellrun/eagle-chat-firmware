#ifndef ROUTING_H_INCLUDED
#define ROUTING_H_INCLUDED

#include "asf.h"
#include "radio/radio.h"

#define PACKET_HEADER_SIZE      3
#define MAX_ACK_FAILURES        10

typedef struct {
    uint8_t source;
    uint8_t dest;
    uint8_t flags;
} __attribute__((packed)) PacketHeader;

#define MAX_PACKET_PAYLOAD_SIZE (MAX_PAYLOAD_SIZE - PACKET_HEADER_SIZE)

typedef struct {
    uint8_t nextHop;
    uint8_t failures;
} RoutingTableEntry;

#define ROUTING_TABLE_ENTRIES   256

//! Consults the routing table and attempts to send a packet
void sendPacket(PacketHeader *h, uint8_t *payload, uint8_t payloadLen);

//! Processes received packets, sends ACKS, forwards packets, etc.
void handleReceived(void);


#endif /*ROUTING_H_INCLUDED*/
