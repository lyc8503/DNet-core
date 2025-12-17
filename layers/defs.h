//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_LAYERS_DEFS_H
#define DNET_LAYERS_DEFS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>


union uint16_be {
    uint8_t bytes[2]{};

    [[nodiscard]] uint16_t val() const {
        return ((uint16_t) bytes[0] << 8) + bytes[1];
    }

    uint16_be &operator=(const uint16_t &val) {
        bytes[1] = val & 0xff;
        bytes[0] = (val & 0xff00) >> 8;

        return *this;
    }

    bool operator==(const uint16_t &val) const {
        return this->val() == val;
    }

    uint16_be() {
        *this = 0;
    }

    explicit uint16_be(uint16_t i) {
        *this = i;
    }
} __attribute__((packed));


union uint32_be {
    uint8_t bytes[4]{};

    [[nodiscard]] uint32_t val() const {
        return ((uint32_t) bytes[0] << 24) + ((uint32_t) bytes[1] << 16) + ((uint32_t) bytes[2] << 8) + bytes[3];
    }

    uint32_be &operator=(const uint32_t &val) {
        bytes[3] = val & 0xff;
        bytes[2] = (val & 0xff00) >> 8;
        bytes[1] = (val & 0xff0000) >> 16;
        bytes[0] = (val & 0xff000000) >> 24;

        return *this;
    }

    bool operator==(const uint32_t &val) const {
        return this->val() == val;
    }

    uint32_be() {
        *this = 0;
    }

    explicit uint32_be(uint32_t i) {
        *this = i;
    }
} __attribute__((packed));


union MacAddress {
    uint8_t bytes[6];

    static MacAddress get_broadcast_address(){
        return MacAddress({{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}});
    }

    std::string to_string() const {
        std::stringstream ss;

        for (int i = 0; i < 6; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) bytes[i] << (i == 5 ? "" : ":");
        }

        return ss.str();
    }

    [[nodiscard]] bool is_broadcast() const {
        return bytes[0] == 0xff &&
               bytes[1] == 0xff &&
               bytes[2] == 0xff &&
               bytes[3] == 0xff &&
               bytes[4] == 0xff &&
               bytes[5] == 0xff;
    }

    MacAddress &operator=(const MacAddress &mac) {
        if (this != &mac) {
            memcpy(this->bytes, mac.bytes, 6);
        }
        return *this;
    }

    void parse_string(const std::string &mac_str) {
        if (sscanf(mac_str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]) != 6) {
            throw std::invalid_argument("Invalid mac address: " + mac_str);
        }
    }
} __attribute__((packed));


union Ipv4Address {
    uint32_be data{};
    uint8_t bytes[4];

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << (int) bytes[0] << "." << (int) bytes[1] << "." << (int) bytes[2] << "." << (int) bytes[3];
        return ss.str();
    }

    void parse_string(const std::string &ip_str) {
        int x1, x2, x3, x4;
        if (sscanf(ip_str.c_str(), "%d.%d.%d.%d", &x1, &x2, &x3, &x4) != 4) {
            throw std::invalid_argument("Invalid ipv4 address: " + ip_str);
        }

        if (x1 >= 0 && x2 >= 0 && x3 >= 0 && x4 >= 0 && x1 <= 255 && x2 <= 255 && x3 <= 255 && x4 <= 255) {
            this->bytes[0] = x1;
            this->bytes[1] = x2;
            this->bytes[2] = x3;
            this->bytes[3] = x4;
        } else {
            throw std::invalid_argument("Invalid ipv4 address: " + ip_str);
        }
    }

    Ipv4Address &operator=(const Ipv4Address &ipv4) {
        if (this != &ipv4) {
            memcpy(bytes, ipv4.bytes, 4);
        }
        return *this;
    }

    bool operator==(const Ipv4Address &val) const {
        return data == val.data.val();
    }
} __attribute__((packed));

struct Ipv4AddressHasher
{
    std::size_t operator()(Ipv4Address const& s) const
    {
        return s.data.val();
    }
};

struct Ipv4Subnet {

    Ipv4Address network;
    Ipv4Address mask;

    bool contains(const Ipv4Address &addr) const {
        return (addr.bytes[0] & mask.bytes[0]) == network.bytes[0] &&
               (addr.bytes[1] & mask.bytes[1]) == network.bytes[1] &&
               (addr.bytes[2] & mask.bytes[2]) == network.bytes[2] &&
               (addr.bytes[3] & mask.bytes[3]) == network.bytes[3];
    }
} __attribute__((packed));

#endif //DNET_LAYERS_DEFS_H
