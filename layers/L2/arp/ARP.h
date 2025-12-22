//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_ARP_H
#define DNET_ARP_H


#include <cstdio>
#include <ctime>
#include <unordered_map>
#include <optional>
#include "../../../defs.h"
#include "../../../DNet.h"

class DNet;

const time_t ARP_CACHE_TTL=120;//in seconds


// RARP is not included
enum class ARP_OPCODE {
    ARP_REQUEST = 1, ARP_RESPONSE = 2
};

std::ostream &operator<<(std::ostream &os, ARP_OPCODE op);


// https://zh.wikipedia.org/zh-sg/%E5%9C%B0%E5%9D%80%E8%A7%A3%E6%9E%90%E5%8D%8F%E8%AE%AE
struct ArpPayload {
    uint16_be htype;
    uint16_be ptype;
    uint8_t hlen;       // Must be 6 here (ethernet), otherwise change the definitions below.
    uint8_t plen;       // Must be 4 here (ipv4)
    uint16_be opcode;

    MacAddress sender_mac;
    Ipv4Address sender_ip;
    MacAddress target_mac;
    Ipv4Address target_ip;

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;

        ss << "ARP Payload [op=" << (ARP_OPCODE) opcode.val();
        ss << ", sender_mac=" << sender_mac.to_string() << ", sender_ip=" << sender_ip.to_string();
        ss << ", target_mac=" << target_mac.to_string() << ", target_ip=" << target_ip.to_string() << "]";
        return ss.str();
    }
} __attribute__((packed));

struct ArpCacheEntry{
    MacAddress address{};
    time_t last_update{};
    ArpCacheEntry() = default;
    explicit ArpCacheEntry(MacAddress _address, time_t _last_update):address(_address), last_update(_last_update){}
};

class ARP {
public:

    explicit ARP(DNet& context);

    ssize_t send_response(MacAddress sender_mac, Ipv4Address sender_ip, MacAddress target_mac, Ipv4Address target_ip);

    ssize_t send_request(MacAddress sender_mac, Ipv4Address sender_ip, MacAddress target_mac, Ipv4Address target_ip);

    void on_recv(void *buf, size_t size);

    std::optional<MacAddress> lookup(Ipv4Address);

private:
    DNet& context;

    std::unordered_map<Ipv4Address, ArpCacheEntry> cache;

};


#endif //DNET_ARP_H
