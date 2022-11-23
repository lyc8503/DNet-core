//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_L2_H
#define DNET_L2_H

#include <cstdint>
#include "../Layer.h"
#include "../defs.h"
#include "../../DNet.h"

class DNet;

enum class EtherType {
    ARP = 0x0806,
    IP = 0x0800
};

std::ostream &operator<<(std::ostream &os, EtherType t);


// https://en.wikipedia.org/wiki/Ethernet_frame
struct EthernetFrame {
    MacAddress dest_mac;
    MacAddress src_mac;
    uint16_be ether_type;
    uint8_t payload[0];

    std::string to_string() const {
        std::stringstream ss;
        ss << "EthernetFrame [dest_mac=" << dest_mac.to_string() << ", src_mac=" << src_mac.to_string() << ", ether_type=" << (EtherType) ether_type.val() << "]";
        return ss.str();
    }
} __attribute__((packed));

static_assert(sizeof(EthernetFrame) == 14);

class L2 : Layer {

public:
    explicit L2(DNet& context);

    ssize_t send(void *buf, size_t size, MacAddress dest, EtherType type);

    void on_recv(void *buf, size_t size) override;

    class ARP *arp;

private:
    DNet& context;
};


#endif //DNET_L2_H
