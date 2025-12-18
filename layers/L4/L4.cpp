//
// Created by lyc8503 on 2022/10/24.
//


#include "L4.h"
#include "../../defs.h"
#include "udp/UDP.h"

L4::L4(DNet &context) : context(context) {
    udp = new UDP(context);
}

void L4::on_recv(void *buf, size_t size, L3Context l3_context) {
    switch (l3_context.protocol) {
        case IPV4_PROTOCOL::UDP: {
            udp->on_recv(buf, size, l3_context);
            break;
        }
        default:
            DNET_ASSERT(false, "Unknown ipv4 protocol: " + int_to_hex((uint8_t) l3_context.protocol));
    }
}

size_t L4::send(void *buf, size_t size, L3Context l3_context) {
    this->context.L3_send(l3_context.src_ip, l3_context.dest_ip, l3_context.protocol, buf, size);
    return 0;
}

