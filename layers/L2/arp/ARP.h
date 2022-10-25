//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_ARP_H
#define DNET_ARP_H


#include <cstdio>
#include "../../defs.h"
#include "../../../DNet.h"

class DNet;


// RARP is not included
enum ARP_OPCODE {
    ARP_REQUEST = 1, ARP_RESPONSE = 2
};

std::ostream &operator<<(std::ostream &os, ARP_OPCODE op);


// https://zh.wikipedia.org/zh-sg/%E5%9C%B0%E5%9D%80%E8%A7%A3%E6%9E%90%E5%8D%8F%E8%AE%AE
struct ArpPayload {
    uint16_be htype;
    uint16_be ptype;
    uint8_t hlen;       // Must be 6 here (ethernet), otherwise change the definitions below.
    uint8_t plen;       // Must be 4 here (ipv4)
    uint16_be opcode;

    MacAddress sender_mac;
    Ipv4Address sender_ip;
    MacAddress target_mac;
    Ipv4Address target_ip;

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;

        ss << "ARP Payload [op=" << (ARP_OPCODE) opcode.val();
        ss << ", sender_mac=" << sender_mac.to_string() << ", sender_ip=" << sender_ip.to_string();
        ss << ", target_mac=" << target_mac.to_string() << ", target_ip=" << target_ip.to_string() << "]";
        return ss.str();
    }
} __attribute__((packed));


class ARP {
public:

    explicit ARP(DNet *context);

    ssize_t send_response(MacAddress sender_mac, Ipv4Address sender_ip, MacAddress target_mac, Ipv4Address target_ip);

    void on_recv(void *buf, size_t size);

private:
    DNet *context;

};


#endif //DNET_ARP_H
