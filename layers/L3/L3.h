//
// Created by lyc8503 on 2022/10/24.
//

#ifndef DNET_L3_H
#define DNET_L3_H

#include <cstdint>
#include "../defs.h"
#include "../../DNet.h"

class DNet;

enum IPV4_PROTOCOL {
    ICMP = 1, IGMP = 2, TCP = 6, UDP = 17, ENCAP = 41, OSPF = 89, SCTP = 132
};

std::ostream &operator<<(std::ostream &os, IPV4_PROTOCOL t);


// https://zh.wikipedia.org/wiki/IPv4
struct Ipv4Packet {
    uint8_t ihl: 4;
    uint8_t version: 4;
    uint8_t ecn: 2;
    uint8_t ds: 6;
    uint16_be total_len;
    uint16_be identification;
    uint16_t fragment_offset: 13;
    uint8_t flags: 3;
    uint8_t ttl;
    uint8_t protocol;
    uint16_be header_checksum;
    Ipv4Address src_ip;
    Ipv4Address dest_ip;
    uint8_t data[0];

    std::string to_string() {
        std::stringstream ss;

        ss << "Ipv4Packet [version=" << (uint32_t) version << ", ihl=" << (uint32_t) ihl;
        ss << ", ds_and_ecn=" << std::hex << (uint32_t) ds << ", " << (uint32_t) ecn << std::dec << ", total_len=" << total_len.val();
        ss << ", identification=" << identification.val() << ", flags_and_fragment_offset=" << std::hex << (uint32_t) flags << ", " << fragment_offset << std::dec;
        ss << ", ttl=" << (uint32_t) ttl << ", protocol=" << (IPV4_PROTOCOL) protocol << ", header_checksum=" << std::hex << header_checksum.val() << std::dec;
        ss << ", src_ip=" << src_ip.to_string() << ", dest_ip=" << dest_ip.to_string();
        ss << "]";

        return ss.str();
    }
} __attribute__((packed));

static_assert(sizeof(Ipv4Packet) == 20);

class L3 {
public:

    explicit L3(DNet* context);

    ssize_t send(void *buf, size_t size);
    void on_recv(void *buf, size_t size);

private:
    DNet* context;
};


#endif //DNET_L3_H
