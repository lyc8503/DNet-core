//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include "ARP.h"
#include "../../../defs.h"


std::ostream &operator<<(std::ostream &os, ARP_OPCODE op) {
    switch (op) {
        case ARP_OPCODE::ARP_REQUEST :
            return os << "ARP_REQUEST";
        case ARP_OPCODE::ARP_RESPONSE:
            return os << "ARP_RESPONSE";
        default:
            return os << "Unknown(" << (uint32_t) op << ")";
    }
}

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
    return this->context->L2_send(&payload, sizeof(payload), target_mac);
}

ssize_t ARP::send_request(MacAddress sender_mac, Ipv4Address sender_ip, MacAddress target_mac, Ipv4Address target_ip) {

    ArpPayload payload{};

    payload.htype = 0x0001;  // Hardcoded, ethernet
    payload.ptype = 0x0800;  // Hardcoded, ipv4
    payload.hlen = 6;
    payload.plen = 4;
    payload.opcode = ARP_OPCODE::ARP_REQUEST;
    payload.sender_mac = sender_mac;
    payload.sender_ip = sender_ip;
    payload.target_mac = target_mac;
    payload.target_ip = target_ip;

    DNET_DEBUG("Send ARP request: %s", payload.to_string().c_str());

    // TODO: actual size of data sent
    return this->context->L2_send(&payload, sizeof(payload), target_mac);
}


void ARP::on_recv(void *buf, size_t size) {
    // Some implementations will fill zeros after the payload, so the size is bigger than ArpPayload(28).
            DNET_ASSERT(size >= sizeof(ArpPayload));

    auto *payload = (ArpPayload *) buf;
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
            //Put it into cache
            cache[payload->sender_ip] = ArpCacheEntry(payload->sender_mac,time(nullptr));
            DNET_DEBUG("cache[%s] is now: %s", payload->sender_ip.to_string().c_str(), cache[payload->sender_ip].address.to_string().c_str());
            break;
        default:
            DNET_ERROR("Unknown ARP opcode: %d", payload->opcode.val());
    }


}

std::optional<MacAddress> ARP::lookup(Ipv4Address address){
    DNET_DEBUG("ARP lookup invoked: %s", address.to_string().c_str());
    auto entry = cache.find(address);
    if(entry==cache.end() || entry->second.last_update + ARP_CACHE_TTL < time(nullptr)) {
        //not in cache or has expired
        DNET_DEBUG("ARP cache missed. Sending request");
        send_request(context->mac(), context->subnet().network, MacAddress::get_broadcast_address(), address);
        return {};
    }
    DNET_DEBUG("ARP cache hit. %s is %s.", address.to_string().c_str(), entry->second.address.to_string().c_str());
    return entry->second.address;
}

ARP::ARP(DNet *context) {
    this->context = context;
}
