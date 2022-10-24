//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include "ARP.h"
#include "../../../defs.h"

ssize_t ARP::send_response(MacAddress sender_mac, Ipv4Address sender_ip, MacAddress target_mac, Ipv4Address target_ip) {

    ArpPayload payload{};

    payload.htype = 0x0001;  // Hardcoded, ethernet
    payload.ptype = 0x0800;  // Hardcoded, ipv4
    payload.hlen = 6;
    payload.plen = 4;
    payload.opcode = ARP_OPCODE::ARP_RESPONSE;
    payload.sender_mac = sender_mac;
    payload.sender_ip = sender_ip;
    payload.target_mac = target_mac;
    payload.target_ip = target_ip;

    DNET_DEBUG("Send ARP response: %s", payload.to_string().c_str());

    // TODO: actual size of data sent
    return this->context->ethernet_layer->send(&payload, sizeof(payload), target_mac);
}

void ARP::on_recv(void *buf, size_t size) {
    // Some implementations will fill zeros after the payload, so the size is bigger than ArpPayload(28).
    DNET_ASSERT(size >= sizeof(ArpPayload));

    auto* payload = (ArpPayload*) buf;
    DNET_ASSERT(payload->htype == 0x0001);  // Hardcoded, ethernet
    DNET_ASSERT(payload->ptype == 0x0800);  // Hardcoded, ipv4
    DNET_ASSERT(payload->hlen == 6);  // Hardware address length (mac)
    DNET_ASSERT(payload->plen == 4);  // Protocol address length (ipv4)

    DNET_DEBUG("Received ARP: %s", payload->to_string().c_str());

    switch (payload->opcode.val()) {
        case ARP_REQUEST:
            if (context->subnet().contains(payload->target_ip)) {
                // The sender is now the target
                send_response(context->mac(), payload->target_ip, payload->sender_mac, payload->sender_ip);
            }
            break;
        case ARP_RESPONSE:

            break;
        default:
            DNET_ERROR("Unknown ARP opcode: %d", payload->opcode.val());
    }


}

ARP::ARP(DNet *context) {
    this->context = context;
}
