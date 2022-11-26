//
// Created by lyc8503 on 2022/10/12.
//

#ifndef DNET_UTIL_H
#define DNET_UTIL_H

#include <cstdint>
#include "../layers/defs.h"

uint16_t checksum_16bit_be(const void *src, size_t len);

template<typename T>
std::string int_to_hex(T i) {
    std::stringstream stream;
    stream << "0x"
           << std::setfill('0') << std::setw(sizeof(T) * 2)
           << std::hex << (uint32_t) i;
    return stream.str();
}

#endif //DNET_UTIL_H
