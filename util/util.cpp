//
// Created by lyc8503 on 2022/10/12.
//

#include "util.h"
#include "../defs.h"


void print_hex(uint8_t* buf, int size) {
    for (int i = 0; i < size; i++) {
        printf("%x ", buf[i]);
    }
}
