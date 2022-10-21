//
// Created by lyc8503 on 2022/10/20.
//

#ifndef DNET_LAYERS_DEFS_H
#define DNET_LAYERS_DEFS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

#pragma pack(1)

union uint16_be {
    uint8_t bytes[2];

    uint16_t val() {
        return ((uint16_t) bytes[0] << 8) + bytes[1];
    }
};

union uint32_be {
    uint8_t bytes[4];

    uint32_t val() {
        return ((uint32_t) bytes[0] << 24) + ((uint32_t) bytes[1] << 16) + ((uint32_t) bytes[2] << 8) + bytes[3];
    }
};

union MacAddress {
    uint8_t bytes[6];

    std::string to_string() {
        std::stringstream ss;

        for (int i = 0; i < 6; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) bytes[i] << (i == 5 ? "" : ":");
        }

        return ss.str();
    }

    bool is_broadcast() {
        return bytes[0] == 0xff &&
               bytes[1] == 0xff &&
               bytes[2] == 0xff &&
               bytes[3] == 0xff &&
               bytes[4] == 0xff &&
               bytes[5] == 0xff;
    }
};

union Ipv4Address {
    uint32_be data;
    uint8_t bytes[4];

    std::string to_string() {
        std::stringstream ss;
        ss << (int) bytes[0] << "." << (int) bytes[1] << "."<< (int) bytes[2] << "."<< (int) bytes[3];
        return ss.str();
    }
};

#pragma pack()


#endif //DNET_LAYERS_DEFS_H
