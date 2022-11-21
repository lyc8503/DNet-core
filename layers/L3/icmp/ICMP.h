//
// Created by lyc8503 on 2022/11/20.
//

#ifndef DNET_ICMP_H
#define DNET_ICMP_H

#include "../../defs.h"
#include "../../../DNet.h"

class ICMP {
public:
    explicit ICMP(DNet& context);

    void on_recv(void* buf, size_t size);

private:
    DNet& context;
};

// https://zh.wikipedia.org/wiki/%E4%BA%92%E8%81%94%E7%BD%91%E6%8E%A7%E5%88%B6%E6%B6%88%E6%81%AF%E5%8D%8F%E8%AE%AE
struct ICMPDatagram {
    uint8_t type;
    uint8_t code;
    uint16_be checksum;
    uint32_be rest_of_header;

    uint8_t data[0];

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << "ICMPDatagram [type=" << type << ", code=" << code << ", checksum=" << checksum.val() << ", rest_of_header=" << rest_of_header.val() << "]";
        return ss.str();
    }
} __attribute__((packed));

#endif //DNET_ICMP_H
