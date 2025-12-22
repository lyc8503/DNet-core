
#ifndef DNET_TCP_H
#define DNET_TCP_H


#include "../../../DNet.h"
#include "../../../defs.h"
#include <memory>

struct L3Context;

#define WINDOW_SIZE 65535


// https://en.wikipedia.org/wiki/Transmission_Control_Protocol
struct TcpSegment {
    uint16_be src_port;
    uint16_be dest_port;
    uint32_be seq;
    uint32_be ack;
    uint8_t data_offset;

    union {
        uint8_t flags;
        struct {
            uint8_t fin: 1;
            uint8_t syn: 1;
            uint8_t rst: 1;
            uint8_t psh: 1;
            uint8_t ack: 1;
            uint8_t urg: 1;
            uint8_t ece: 1;
            uint8_t cwr: 1;
        } flag_bits;
    };

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


enum TcpState {
    LISTEN, SYN_SENT, SYN_RECVED, ESTABLISHED, FIN_WAIT_1, FIN_WAIT_2, CLOSING_WAIT, CLOSING, LAST_ACK, TIME_WAIT, CLOSED
};


struct TcpConnTuple {
    Ipv4Address src_ip;
    uint16_t src_port;
    Ipv4Address dest_ip;
    uint16_t dest_port;

    std::string to_string() const {
        std::stringstream ss;
        ss << src_ip.to_string() << ":" << src_port << "->" 
           << dest_ip.to_string() << ":" << dest_port;
        return ss.str();
    }

    bool operator==(const TcpConnTuple &other) const {
        return src_ip == other.src_ip && src_port == other.src_port &&
               dest_ip == other.dest_ip && dest_port == other.dest_port;
    }
};


namespace std {
    template<>
    struct hash<TcpConnTuple> {
        std::size_t operator()(const TcpConnTuple &k) const {
            std::size_t seed = 0;
            hash_combine(seed, k.src_ip.data.val());
            hash_combine(seed, k.src_port);
            hash_combine(seed, k.dest_ip.data.val());
            hash_combine(seed, k.dest_port);
            // DNET_DEBUG("Hashing TcpConnTuple %s to %zu", k.to_string().c_str(), seed);
            return seed;
        }
    };
}


struct TcpConn {
    TcpConnTuple tuple;
    TcpState state;

    // RFC793
    uint32_t snd_una;  // send unacknowledged
    uint32_t snd_nxt;  // send next
    uint32_t rcv_nxt;  // receive next


    TcpConn() : state(CLOSED) {};
};


class TCP {

public:

    explicit TCP(DNet& context);
    void on_recv(void* buf, size_t size, L3Context l3_context);

private:

    DNet& context;
    std::unordered_map<TcpConnTuple, std::shared_ptr<TcpConn>> connections;
};

#endif //DNET_TCP_H