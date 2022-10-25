//
// Created by lyc8503 on 2022/10/24.
//

#include "L3.h"
#include "../../defs.h"
#include "../../util/util.h"


std::ostream &operator<<(std::ostream &os, IPV4_PROTOCOL t) {
    switch (t) {
        case IPV4_PROTOCOL::ICMP :
            return os << "ICMP";
        case IPV4_PROTOCOL::IGMP:
            return os << "IGMP";
        case IPV4_PROTOCOL::TCP:
            return os << "TCP";
        case IPV4_PROTOCOL::UDP:
            return os << "UDP";
        case IPV4_PROTOCOL::ENCAP:
            return os << "ENCAP";
        case IPV4_PROTOCOL::OSPF:
            return os << "OSPF";
        case IPV4_PROTOCOL::SCTP:
            return os << "SCTP";
        default:
            return os << "Unknown(" << (uint32_t) t << ")";
    }
}


L3::L3(DNet *context) {
    this->context = context;
}

void L3::on_recv(void *buf, size_t size) {

    auto* packet = (Ipv4Packet*) buf;
    DNET_DEBUG("%s", packet->to_string().c_str());

    // Verify version
    DNET_ASSERT(packet->version() == 4);

    // Options field not supported, so the ihl is fixed.
    DNET_ASSERT(packet->ihl() * 4 == sizeof(Ipv4Packet));

    // Verify checksum
    uint16_t checksum = packet->header_checksum.val();
    packet->header_checksum = 0x0000;
    DNET_ASSERT(checksum == checksum_16bit((uint16_be*) buf, sizeof(Ipv4Packet) / 2));



}
