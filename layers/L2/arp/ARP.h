//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_ARP_H
#define DNET_ARP_H


#include <cstdio>
#include "../../defs.h"


#pragma pack(1)

// https://zh.wikipedia.org/zh-sg/%E5%9C%B0%E5%9D%80%E8%A7%A3%E6%9E%90%E5%8D%8F%E8%AE%AE
struct ArpPayload {
    uint16_be htype;
    uint16_be ptype;
    uint8_t hlen;       // Must be 6 here (ethernet), otherwise change the definitions below.
    uint8_t plen;       // Must be 4 here (ipv4)
    uint16_be opcode;

    MacAddress src_mac;
    Ipv4Address src_ip;
    MacAddress dest_mac;
    Ipv4Address dest_ip;
};

#pragma pack()

// RARP is not included
enum ARP_OPCODE {
    ARP_REQUEST = 1, ARP_RESPONSE = 2
};

class ARP {
public:

    ssize_t send(void* buf, size_t size);
    void on_recv(void* buf, size_t size);
};


#endif //DNET_ARP_H
