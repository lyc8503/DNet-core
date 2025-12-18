//
// Created by lyc8503 on 2022/10/24.
//

#ifndef DNET_L4_H
#define DNET_L4_H

#include <cstdint>
#include "../../DNet.h"


struct L3Context;
enum class IPV4_PROTOCOL;

struct PseudoHeader {
    Ipv4Address src_ip;
    Ipv4Address dest_ip;
    uint8_t zero;
    uint8_t protocol;
    uint16_be length;
} __attribute__((packed));


class L4 {
public:

    L4(DNet& context);

    void on_recv(void* buf, size_t size, L3Context l3_context);
    size_t send(void* buf, size_t size, L3Context l3_context);

private:

    DNet& context;
    class UDP* udp;

};

#endif  // DNET_L4_H
