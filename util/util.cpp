//
// Created by lyc8503 on 2022/10/12.
//

#include "util.h"


uint16_t checksum_16bit(const uint16_be *src, std::size_t len) {

    uint32_t ret = 0;

    for (std::size_t i = 0; i < len; i++) {
        ret += (src[i]).val();
    }

    return ~((ret >> 16) + ret & 0xffff);
}
