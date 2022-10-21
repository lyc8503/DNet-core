//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_ARP_H
#define DNET_ARP_H


#include <cstdio>
#include "../../defs.h"


// RARP is not included
enum ARP_OPCODE {
    ARP_REQUEST = 1, ARP_RESPONSE = 2
};

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

    std::string to_string() {
        std::stringstream ss;

        ss << "ARP Payload [op=";

        switch (opcode.val()) {
            case ARP_REQUEST:
                ss << "ARP_REQUEST";
                break;
            case ARP_RESPONSE:
                ss << "ARP_RESPONSE";
                break;
            default:
                break;
        }

        ss << ", src_mac=" << src_mac.to_string() << ", src_ip=" << src_ip.to_string();
        ss << ", dest_mac=" << dest_mac.to_string() << ", dest_ip=" << dest_ip.to_string() << "]";
        return ss.str();
    }
} __attribute__((packed));



class ARP {
public:

    ssize_t send(void* buf, size_t size);
    void on_recv(void* buf, size_t size);

};


#endif //DNET_ARP_H
