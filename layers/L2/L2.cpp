//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include "L2.h"
#include "../../defs.h"
#include "arp/ARP.h"


ssize_t L2::send(void *buf, size_t size, MacAddress dest) {

    // TODO: refactor

    uint8_t tmp[sizeof(EthernetFrame) + size];

    auto* frame = (EthernetFrame*) tmp;
    frame->dest_mac = dest;
    frame->src_mac = this->context->mac();
    frame->ether_type = EtherType::ARP;
    memcpy(frame->payload, buf, size);

    return this->context->dri->write(tmp, sizeof(EthernetFrame) + size);
}

void L2::on_recv(void *buf, size_t size) {
//    DNET_ASSERT(size >= 64 && size <= 1518);

    auto* frame = (EthernetFrame*) buf;

    switch (frame->ether_type.val()) {
        case EtherType::ARP:
            DNET_ASSERT(frame->dest_mac.is_broadcast());
            arp->on_recv(frame->payload, size - ETHERNET_FRAME_HEADER_LEN);
            break;
        case EtherType::IP:
            DNET_DEBUG("IP!");
            break;
        default:
            DNET_DEBUG("Unsupported EtherType for L2: %04x, packet dropped.", frame->ether_type.val());
            break;

    }

}

L2::L2(DNet *context) {
    this->context = context;
    this->arp = new class ARP(context);
}

