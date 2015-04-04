#include "routing.h"
#include "radio.h"
#include <string.h>

static uint8_t framePayload[MAX_PAYLOAD_SIZE];

static RoutingTableEntry routingTable[ROUTING_TABLE_ENTRIES];

static uint8_t _nodeId = 0;

bool getNextHop(uint8_t dest, RoutingTableEntry **nextHopEntry);
bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t payloadLen);

static inline bool entryIsValid(RoutingTableEntry * entry) {
    return (entry->nextHop != 0) && (entry->failures < MAX_ACK_FAILURES);
}

void setupRouting(uint8_t nodeId) {

    _nodeId = nodeId;

    memset(routingTable, 0, sizeof(routingTable)); // init routing table

    routingTable[1].nextHop = 1; // Dummify the routing table
    routingTable[2].nextHop = 2;

    setupRadio();
    setAddress(nodeId);
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

    uint32_t send_time;
    bool acked;
    uint8_t retries = 0;

    while (retries < 5) {
        sendFrame(nextHopEntry->nextHop, framePayload, frameLen);


        acked = false;

        send_time = rtc_get_time();
        while (rtc_get_time() - send_time < 50) {
            if ((acked = ackReceived(nextHopEntry->nextHop))) {
                break;
            }
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
        if (forward(nextHopEntry, framePayload, payloadLen + PACKET_HEADER_SIZE)) {
            // Packet sent
        } else {
            // Do something with failure
        }

    } else {

        // Do the hard part

    }

}

void initiateRouteRequest(uint8_t dest) {

    // We want to send out a route request to find a path to dest

    PacketHeader h;
    h.source = _nodeId;
    h.dest = dest;
    h.flags = 0;
    h.type = PACKET_TYPE_RRQ;

    RRQPacketHeader rh;
    rh.rrqId = 0;
    rh.hopcount = 0;

    memcpy(framePayload, (uint8_t *)h, PACKET_HEADER_SIZE);
    memcpy(&framePayload[PACKET_HEADER_SIZE], (uint8_t *)rh, RRQ_PACKET_HEADER_SIZE);

    // Broadcast it?

    broadcastFrame(&framePayload, PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE);

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

        switch (h->type) {
            case PACKET_TYPE_CONTENT:

                if (h->dest == _nodeId) { // This packet is for us
                    // Should probably stick in a buffer as needed
                    cdc_write_line("Got packet for us");
                    cdc_log_string("Payload: ", &framePayload[PACKET_HEADER_SIZE]);
                    continue;
                }

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

                break;

            case PACKET_TYPE_RRQ:

                if (h->dest == _nodeId) {
                    // We are the destination; Start the ball rolling on a route update

                    
                }

                RRQPacketHeader *rh = (RRQPacketHeader *)&framePayload[PACKET_HEADER_SIZE];

                // Add the rrqId to our list of recently forwarded RRQ's

                uint8_t *nodeList = &framePayload[PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE];
                nodeList[rh->hopcount] = _nodeId;

                rh->hopcount++;

                broadcastFrame(&framePayload, PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE + rh->hopcount);

                break;

            case PACKET_TYPE_RUP:



                break;

        }

    }

}
