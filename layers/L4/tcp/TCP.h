
#ifndef DNET_TCP_H
#define DNET_TCP_H


#include "../../../DNet.h"
#include "../../../defs.h"
#include <memory>

struct L3Context;

#define WINDOW_SIZE 4096
#define BUFFER_SIZE 65536

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


#define TCP_CHECKSUM(segment, seg_src_ip, seg_dst_ip, body, body_len) ({ \
    uint16_t checksum; \
    uint8_t pseudo_header_buf[sizeof(PseudoHeader) + sizeof(TcpSegment) + body_len]; \
    auto* pseudo_header = (PseudoHeader*) pseudo_header_buf; \
    pseudo_header->src_ip = seg_src_ip; \
    pseudo_header->dest_ip = seg_dst_ip; \
    pseudo_header->zero = 0; \
    pseudo_header->protocol = (uint8_t) IPV4_PROTOCOL::TCP; \
    pseudo_header->length = uint16_be(sizeof(TcpSegment) + body_len); \
    memcpy(pseudo_header_buf + sizeof(PseudoHeader), segment, sizeof(TcpSegment)); \
    memcpy(pseudo_header_buf + sizeof(PseudoHeader) + sizeof(TcpSegment), body, body_len); \
    checksum = checksum_16bit_be(pseudo_header_buf, sizeof(PseudoHeader) + sizeof(TcpSegment) + body_len); \
    checksum; \
})


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

struct RingBuffer {
    uint8_t buf[BUFFER_SIZE];

    size_t start;
    size_t end;

    int put(uint8_t octet) {
        if ((end + 1) % BUFFER_SIZE == start) {
            return 0;
        }
        buf[end] = octet;
        end = (end + 1) % BUFFER_SIZE;
        return 1;
    }

    int get(uint8_t* octet) {
        if (start == end) {
            return 0;
        }
        *octet = buf[start];
        start = (start + 1) % BUFFER_SIZE;
        return 1;
    }

    int size() {
        return (end - start + BUFFER_SIZE) % BUFFER_SIZE;
    }

    int capacity() {
        return BUFFER_SIZE - size();
    }

    int writeAll(const uint8_t* buf, size_t len) {
        if (len > capacity()) {
            return 0;
        }
        for (size_t i = 0; i < len; i++) {
            put(buf[i]);
        }
        return 1;
    }

    int readFull(uint8_t* buf, size_t len) {
        if (len > size()) {
            return 0;
        }
        for (size_t i = 0; i < len; i++) {
            get(&buf[i]);
        }
        return 1;
    }

    RingBuffer() : start(0), end(0) {};
};


struct TcpConn {
    TcpConnTuple tuple;
    TcpState state;

    // RFC793
    uint32_t snd_una;  // send unacknowledged
    uint32_t snd_nxt;  // send next
    uint32_t rcv_nxt;  // receive next

    RingBuffer rcvbuf;
    RingBuffer sndbuf;

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