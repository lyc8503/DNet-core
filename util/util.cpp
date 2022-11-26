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
    auto *buf = (uint16_be *) src;

    uint32_t ret = 0;

    while (len > 1) {
        ret += buf->val();
        buf++;
        len -= 2;
    }

    if (len > 0) {
        ret += (*(uint8_t *) buf) << 8;
    }

    ret = ~((ret >> 16) + ret & 0xffff);

    if (ret == 0) {
        return 0xffff;  // If the computed checksum is zero, it is transmitted as all ones
    } else {
        return ret;
    }
}
