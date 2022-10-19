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
union MacAddress {
    uint8_t mac[6];

    std::string to_string() {
        std::stringstream ss;

        for (int i = 0; i < 6; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) mac[i] << (i == 5 ? "" : ":");
        }

        return ss.str();
    }
};
#pragma pack()


#endif //DNET_LAYERS_DEFS_H
