//
// Created by lyc8503 on 2022/10/12.
//

#include "util.h"


/**
 * @brief Calculate the checksum of a 16-bit big-endian array, len in bytes
 * @param src
 * @param len
 * @return
 */
uint16_t checksum_16bit_be(const void *src, size_t len) {
    auto *buf = (const uint16_t *) src;

    uint32_t ret = 0;

    while (len > 1) {
        ret += *buf;
        buf++;
        len -= 2;
    }

    if (len > 0) {
        ret += (*(uint8_t *) buf) << 8;
    }

    ret = ~((ret >> 16) + ret & 0xffff);

    // TODO: If the computed checksum is zero, it is transmitted as all ones
    return reinterpret_cast<uint16_be*>(&ret)->val();
}
