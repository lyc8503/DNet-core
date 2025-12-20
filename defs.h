//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_DEFS_H
#define DNET_DEFS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cassert>
#include "util/util.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DNET_DEBUG(format, args...) printf("[DEBUG %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)
#define DNET_ERROR(format, args...) fprintf(stderr, "[ERR %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)

//#define DNET_DEBUG(format, args...) void(0);
//#define DNET_ERROR(format, args...) void(0);

/**
 *  Overload for Macro functions(DNET_ASSERT): https://stackoverflow.com/questions/16683146/can-macros-be-overloaded-by-number-of-arguments
 */
#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )

#define GET_COUNT( _1, _2, _3, _4, _5, _6 /* ad nauseam */, COUNT, ... ) COUNT
#define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )

#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

#define DNET_ASSERT( ... ) VA_SELECT( DNET_ASSERT, __VA_ARGS__ )

#define DNET_ASSERT_2(expr, msg) do {                                                                                                                   \
    if (!(expr)) {                                                                                                                                      \
        throw std::runtime_error("Assertion failed at [" + std::string(__FILENAME__) + ":" + std::to_string(__LINE__) + "]: " + std::string(msg));      \
    }                                                                                                                                                   \
} while(0)

#define DNET_ASSERT_1(expr) DNET_ASSERT_2(expr, "No msg provided.")


namespace std{
    namespace
    {
        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780
        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, std::get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, std::get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>> 
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {                                              
            size_t seed = 0;                             
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
            return seed;                                 
        }                                              
    };
}


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

#endif //DNET_DEFS_H
