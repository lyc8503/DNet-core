//
// Created by lyc8503 on 2022/9/20.
//

#include <cassert>
#include "EthernetLayer.h"
#include "../util/util.h"


ssize_t EthernetLayer::send(void *buf, size_t size) {
    return 0;
}

void EthernetLayer::on_recv(void *buf, size_t size) {

    assert(size >= 64 && size <= 1518);

    auto* frame = (EthernetFrame*) buf;
    printf("src: ");
    print_hex(frame->src_mac, 6);
    printf("dst: ");
    print_hex(frame->dest_mac, 6);
    printf("\n");

}

EthernetLayer::EthernetLayer() = default;
