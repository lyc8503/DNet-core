
#ifndef DNET_TCP_H
#define DNET_TCP_H


#include "../../../DNet.h"
#include "../../../defs.h"

struct L3Context;


// https://en.wikipedia.org/wiki/Transmission_Control_Protocol
struct TcpSegment {
    uint16_be src_port;
    uint16_be dest_port;
    uint32_be seq;
    uint32_be ack;
    uint8_t data_offset;
    uint8_t flags;
    uint16_be window;
    uint16_be checksum;
    uint16_be urgent_ptr;
    uint8_t options_and_data[];

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << "TcpSegment [src_port=" << src_port.val() << ", dest_port=" << dest_port.val();
        ss << ", seq=" << seq.val() << ", ack=" << ack.val();
        ss << ", data_offset=" << (uint32_t) data_offset << ", flags=" << (uint32_t) flags;
        ss << ", window=" << window.val() << ", checksum=" << int_to_hex(checksum.val());
        ss << ", urgent_ptr=" << urgent_ptr.val() << "]";
        return ss.str();
    }
} __attribute__((packed));


class TCP {

public:

    explicit TCP(DNet& context);
    void on_recv(void* buf, size_t size, L3Context l3_context);

private:

    DNet& context;
};

#endif //DNET_TCP_H