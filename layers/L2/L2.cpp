//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include "L2.h"
#include "../../defs.h"
#include "arp/ARP.h"

std::ostream &operator<<(std::ostream &os, EtherType t) {
    switch (t) {
        case EtherType::ARP :
            return os << "ARP";
        case EtherType::IP:
            return os << "IP";
        default:
            std::ios_base::fmtflags f(std::cout.flags());  // save flags
            std::ostream& ret = os << std::hex << "Unknown(" << (uint32_t) t << ")";
            std::cout.flags(f);  // restore flags
            return ret;
    }
}


ssize_t L2::send(void *buf, size_t size, MacAddress dest) {

    // TODO: refactor

    uint8_t tmp[sizeof(EthernetFrame) + size];

    auto* frame = (EthernetFrame*) tmp;
    frame->dest_mac = dest;
    frame->src_mac = this->context.mac();
    frame->ether_type = EtherType::ARP;
    memcpy(frame->payload, buf, size);

    return this->context.driver_send(tmp, sizeof(EthernetFrame) + size);
}

void L2::on_recv(void *buf, size_t size) {
//    DNET_ASSERT(size >= 64 && size <= 1518);

    auto* frame = (EthernetFrame*) buf;

    DNET_DEBUG("%s", frame->to_string().c_str());

    switch (frame->ether_type.val()) {
        case EtherType::ARP:
//            DNET_ASSERT(frame->dest_mac.is_broadcast());
            arp->on_recv(frame->payload, size - sizeof(EthernetFrame));
            break;
        case EtherType::IP:
            context.L3_on_recv(frame->payload, size - sizeof(EthernetFrame));
            break;
        default:
            DNET_DEBUG("Unsupported EtherType for L2: %04x, packet dropped.", frame->ether_type.val());
            break;

    }

}

L2::L2(DNet &context): context(context), arp(new class ARP(context)){
}


