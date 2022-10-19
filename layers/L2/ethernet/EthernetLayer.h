//
// Created by lyc8503 on 2022/9/20.
//

#ifndef DNET_ETHERNETLAYER_H
#define DNET_ETHERNETLAYER_H

#include "../../Layer.h"
#include "../../../driver/driver.h"
#include "../../defs.h"


#pragma pack(1)
// https://en.wikipedia.org/wiki/Ethernet_frame
struct EthernetFrame {
    MacAddress dest_mac;
    MacAddress src_mac;
    uint16_t ether_type;
    uint8_t payload_and_checksum[0];
};
#pragma pack()

enum EtherType {
    ARP = 0x0806,
    IP = 0x0800
};

class EthernetLayer: Layer{

public:
    explicit EthernetLayer();

    ssize_t send(void* buf, size_t size) override;
    void on_recv(void* buf, size_t size) override;
};


#endif //DNET_ETHERNETLAYER_H
