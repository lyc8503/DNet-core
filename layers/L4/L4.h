//
// Created by lyc8503 on 2022/10/24.
//

#ifndef DNET_L4_H
#define DNET_L4_H

#include <cstdint>
#include "../L3/L3.h"
#include "../../util/util.h"


struct L3Context;
enum class IPV4_PROTOCOL;

struct PseudoHeader {
    Ipv4Address src_ip;
    Ipv4Address dest_ip;
    uint8_t zero;
    uint8_t protocol;
    uint16_be length;
} __attribute__((packed));

// https://en.wikipedia.org/wiki/User_Datagram_Protocol
struct UdpDatagram {
    uint16_be src_port;
    uint16_be dest_port;
    uint16_be length;
    uint16_be checksum;
    uint8_t data[];

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << "UdpDatagram [src_port=" << src_port.val() << ", dest_port=" << dest_port.val();
        ss << ", length=" << length.val() << ", checksum=" << int_to_hex(checksum.val()) << "]";
        return ss.str();
    }
} __attribute__((packed));

class L4 {
public:

    L4(DNet& context);

    void on_recv(void* buf, size_t size, L3Context l3_context);
    size_t send(void* buf, size_t size, L3Context l3_context);

private:

    DNet& context;

};

#endif  // DNET_L4_H
