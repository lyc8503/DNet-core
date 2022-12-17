//
// Created by lyc8503 on 2022/10/24.
//

#include <optional>
#include "L3.h"
#include "../../defs.h"
#include "../../util/util.h"
#include "icmp/ICMP.h"


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


L3::L3(DNet& context): context(context) {
    icmp = new ICMP(context);
}

void L3::on_recv(void *buf, size_t size) {

    auto* packet = (Ipv4Packet*) buf;

    DNET_DEBUG("L3 recv: %s", packet->to_string().c_str());

    // Verify version
    DNET_ASSERT(packet->version == 4, "Ipv4 Packet version mismatch.");

    // Options field not supported, so the ihl is fixed.
    DNET_ASSERT(packet->ihl * 4 == sizeof(Ipv4Packet), "Ipv4 Options field not implemented.");

    // Verify checksum
    DNET_ASSERT(checksum_16bit_be(buf, sizeof(Ipv4Packet)) == 0, "Checksum Mismatch.");

    // TODO: process left args of ipv4 packet.

    switch ((IPV4_PROTOCOL) packet->protocol) {
        case IPV4_PROTOCOL::ICMP:
            icmp->on_recv(packet->data, size - packet->ihl * 4, packet->src_ip, packet->dest_ip);
            break;
        default: {
            struct L3Context l3_context;
            l3_context.src_ip = packet->src_ip;
            l3_context.dest_ip = packet->dest_ip;
            l3_context.protocol = (IPV4_PROTOCOL) packet->protocol;

            context.L4_on_recv(packet->data, size - packet->ihl * 4, l3_context);
        }
//            DNET_ASSERT(false, "Unknown Ipv4 Protocol: " + std::to_string(packet->protocol));
    }

}

ssize_t L3::send(Ipv4Address src, Ipv4Address target, void* buf, size_t size) {
    char tmp[sizeof(Ipv4Packet) + size]{};
    auto* packet = (Ipv4Packet*) tmp;
    
    // Set packet headers
    packet->version = 4;
    packet->ihl = sizeof(Ipv4Packet) / 4;
    packet->protocol = static_cast<uint8_t>(IPV4_PROTOCOL::UDP);  // TODO: correct protocol!
    packet->src_ip = src;
    packet->dest_ip = target;
    packet->total_len = sizeof(Ipv4Packet) + size;
    packet->ttl = 64;
    packet->header_checksum = 0x0000;
    packet->header_checksum = checksum_16bit_be(packet, sizeof(Ipv4Packet));


    // TODO: more args
    memcpy(packet->data, buf, size);

    DNET_DEBUG("L3 send: %s", packet->to_string().c_str());

    std::optional<MacAddress> addr = context.arp_lookup(target);

//    DNET_ASSERT(addr.has_value(), "Arp cache missing.");

    if (addr.has_value()) {
        context.L2_send(tmp, sizeof(Ipv4Packet) + size, addr.value(), EtherType::IP);
    }

    return 0;  // TODO
}


