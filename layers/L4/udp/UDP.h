//
// Created by lyc8503 on 2022/12/11.
//

#ifndef DNET_UDP_H
#define DNET_UDP_H

#include "../../../DNet.h"
#include "../../../defs.h"

struct L3Context;

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


class UDP {

public:

    explicit UDP(DNet& context);
    void on_recv(void* buf, size_t size, L3Context l3_context);

private:

    DNet& context;
};


#endif //DNET_UDP_H
