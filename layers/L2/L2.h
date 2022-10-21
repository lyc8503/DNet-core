//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_L2_H
#define DNET_L2_H
#include <cstdint>
#include "../Layer.h"
#include "../defs.h"


#define ETHERNET_FRAME_HEADER_LEN 14

#pragma pack(1)
// https://en.wikipedia.org/wiki/Ethernet_frame
struct EthernetFrame {
    MacAddress dest_mac;
    MacAddress src_mac;
    uint16_be ether_type;
    uint8_t payload_and_checksum[0];
};
#pragma pack()

enum EtherType {
    ARP = 0x0806,
    IP = 0x0800
};

class L2: Layer{

public:
    explicit L2();

    ssize_t send(void* buf, size_t size) override;
    void on_recv(void* buf, size_t size) override;
};


#endif //DNET_L2_H