#ifndef DNET_DNS_H
#define DNET_DNS_H

#include <cstdint>
#include "../../DNet.h"

#define TYPE_A 1
#define TYPE_NS 2
#define TYPE_CNAME 5
#define TYPE_SOA 6
#define TYPE_PTR 12
#define TYPE_MX 15
#define TYPE_TXT 16
#define QTYPE_ALL 255

#define CLASS_IN 1

#define DNS_DEFAULT_TTL 60  // 1min
#define DNS_AUTHORITATIVE_ANSWER 1

typedef std::tuple<uint16_t, std::string> record_key;
typedef std::tuple<uint16_t, uint8_t*> record_value;

#define REGISTER_A_RECORD(name, ip) \
do { \
    Ipv4Address addr; \
    addr.parse_string(ip); \
    record_key key = std::make_tuple(TYPE_A, std::string(name)); \
    uint32_t ip_val = addr.data.val(); \
    uint8_t* data = new uint8_t[4]; \
    memcpy(data, &ip_val, 4); \
    record_value value = std::make_tuple(4, data); \
    dns_records[key] = value; \
} while(0)

struct DNSHeader {
    uint16_be id;

    // bit-fields order is implementation-defined in C++
    // it seems it's from LSB to MSB on most x86_64 systems
    // flags byte 1
    uint8_t rd: 1;  // recursion desired
    uint8_t tc: 1;  // truncated
    uint8_t aa: 1;  // authoritative answer
    uint8_t opcode: 4;  // standard query(0), inverse query(1), server status request(2)
    uint8_t qr: 1;  // query(0), response(1)

    // flags byte 2
    uint8_t rcode: 4;  // response code
    uint8_t reserved: 3;  // reserved, must be 0
    uint8_t ra: 1;  // recursion available

    uint16_be qdcount;  // number of question entries
    uint16_be ancount;  // number of answer entries
    uint16_be nscount;  // number of authority entries
    uint16_be arcount;  // number of resource entries

    std::string to_string() {
        std::stringstream ss;
        ss << "DNSHeader [id=" << id.val() << ", qr=" << (uint32_t) qr << ", opcode=" << (uint32_t) opcode;
        ss << ", aa=" << (uint32_t) aa << ", tc=" << (uint32_t) tc << ", rd=" << (uint32_t) rd;
        ss << ", ra=" << (uint32_t) ra << ", rcode=" << (uint32_t) rcode;
        ss << ", qdcount=" << qdcount.val() << ", ancount=" << ancount.val() << ", nscount=" << nscount.val() << ", arcount=" << arcount.val();
        ss << "]";
        return ss.str();
    }
} __attribute__((packed));

struct DNSPacket {
    DNSHeader header;
    // Question/Answer/Authoritiy/Additional follow
    uint8_t data[];
} __attribute__((packed));

struct DNSResourceRecord {
    uint16_be name;  // name (pointer to domain in question)
    uint16_be type;
    uint16_be class_;
    uint32_be ttl;  // seconds
    uint16_be rdlength;
    uint8_t rdata[];  // variable length RDATA
} __attribute__((packed));

class DNS {
public:

    DNS(DNet& dnet);

    void on_recv(void* buf, size_t size, Ipv4Address src_addr, uint16_t src_port, Ipv4Address dest_addr, uint16_t dest_port);

private:

    DNet& dnet;
    std::unordered_map<record_key, record_value> dns_records;
};

#endif  // DNET_DNS_H
