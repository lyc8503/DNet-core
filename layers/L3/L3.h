//
// Created by lyc8503 on 2022/10/24.
//

#ifndef DNET_L3_H
#define DNET_L3_H

#include <cstdint>
#include "../defs.h"

// https://zh.wikipedia.org/wiki/IPv4
struct Ipv4Packet {
    uint8_t version_and_ihl;
    uint8_t ds_and_ecn;
    uint16_be total_len;
    uint16_be identification;
    uint16_be flags_and_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_be header_checksum;
    Ipv4Address src_ip;
    Ipv4Address dest_ip;
    uint8_t options_and_data[0];
} __attribute__((packed));

enum IPV4_PROTOCOL {
    ICMP = 1, IGMP = 2, TCP = 6, UDP = 17, ENCAP = 41, OSPF = 89, SCTP = 132
};

class L3 {

};


#endif //DNET_L3_H
