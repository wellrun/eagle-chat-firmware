#include "routing.h"
#include <string.h>

static uint8_t framePayload[MAX_PAYLOAD_SIZE];

static RoutingTableEntry routingTable[ROUTING_TABLE_ENTRIES];

void sendPacket(PacketHeader *h, uint8_t *payload, uint8_t payloadLen) {

    uint8_t *harray = (uint8_t *)h;
    memcpy(framePayload, h, PACKET_HEADER_SIZE);
    memcpy(&framePayload[PACKET_HEADER_SIZE], payload, payloadLen);

    uint8_t i = 0;
    bool entryFound = false;
    RoutingTableEntry r;
    while (i < ROUTING_TABLE_ENTRIES) {
        r = routingTable[i];
        if (r.dest == h->dest) {
            entryFound = true;
            break;
        }
    }

    uint8_t send_time;
    bool acked;
    if (entryFound) {

        uint8_t retries = 0;

        while (retries < 5) {
            sendFrame(r.nextHop, &framePayload, payloadLen + PACKET_HEADER_SIZE);


            send_time = rtc_get_time();
            acked = false;
            while (rtc_get_time() - send_time < 50) {
                if ((acked = ackReceived(r.nextHop)))
                    break;
            }

            if (acked) {
                break;
            }

            retries ++;
        }

    } else {

        // Do the hard part

    }

}

void rout() {

    // Do the other hard part

}
