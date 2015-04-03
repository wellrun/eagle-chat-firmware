#include "routing.h"
#include <string.h>

static uint8_t framePayload[MAX_PAYLOAD_SIZE];

static RoutingTableEntry routingTable[ROUTING_TABLE_ENTRIES];

bool getNextHop(uint8_t dest, RoutingTableEntry **nextHopEntry);
bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t payloadLen);

static inline bool entryIsValid(RoutingTableEntry * entry) {
    return (entry->nextHop != 0) && (entry->failures < MAX_ACK_FAILURES);
}

/*
* Attempts to get the routing table entry for the path to dest
* If an invalid entry is found, returns false
* Entries are invalid if the nextHop field is 0 or ack failures equals or
* exceeds MAX_ACK_FAILURES
*/
bool getNextHop(uint8_t dest, RoutingTableEntry **nextHopEntry) {

    RoutingTableEntry *r = &routingTable[dest];

    if (entryIsValid(r)) {

        *nextHopEntry = r;
        return true;

    }

    return false;
}

bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t frameLen) {

    uint8_t send_time;
    bool acked;
    uint8_t retries = 0;

    while (retries < 5) {
        sendFrame(nextHopEntry->nextHop, framePayload, frameLen);


        send_time = rtc_get_time();
        acked = false;
        while (rtc_get_time() - send_time < 50) {
            if ((acked = ackReceived(nextHopEntry->nextHop)))
                break;
        }

        if (acked) {                // node is alive
            nextHopEntry->failures = 0;  // reset failure count
            return true;
        }

        retries ++;
    }

    return false;
}

void sendPacket(PacketHeader *h, uint8_t *payload, uint8_t payloadLen) {

    // Treat the packet header as a byte array
    uint8_t *harray = (uint8_t *)h;

    // Put header and payload into framePayload
    memcpy(framePayload, harray, PACKET_HEADER_SIZE);
    memcpy(&framePayload[PACKET_HEADER_SIZE], payload, payloadLen);

    // Try to find the next hop to the destination
    RoutingTableEntry *nextHopEntry;
    bool entryFound = getNextHop(h->dest, &nextHopEntry);

    if (entryFound) {

        // Attempt to forward the packet
        forward(nextHopEntry, framePayload, payloadLen + PACKET_HEADER_SIZE);

    } else {

        // Do the hard part

    }

}

void handleReceived() {

    uint8_t senderNodeId, frameLength;
    bool needsAck;

    while (framesToRead()) {

        // Get the raw frame
        getNextFrame(&senderNodeId, &frameLength, framePayload, &needsAck);

        // Send an ack if needed
        if (needsAck) {
            sendAck(senderNodeId);
        }

        // Unpack the packet header from the raw frame
        PacketHeader *h = (PacketHeader *)framePayload;

        // Realistically, here we'll want to update our own routing tables
        // with information about h->source

        // Get the next hop to the packet's destination
        RoutingTableEntry *nextHopEntry;
        bool entryFound = getNextHop(h->dest, &nextHopEntry); // pointers having pointers, such a shame

        if (entryFound) {

            forward(nextHopEntry, framePayload, frameLength);

        } else {

            // Do the hard part

        }

    }

}
