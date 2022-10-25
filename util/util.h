//
// Created by lyc8503 on 2022/10/12.
//

#ifndef DNET_UTIL_H
#define DNET_UTIL_H

#include <cstdint>
#include "../layers/defs.h"

uint16_t checksum_16bit(const uint16_be* src, std::size_t len);

#endif //DNET_UTIL_H
