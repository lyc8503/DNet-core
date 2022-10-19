//
// Created by lyc8503 on 2022/9/20.
//

#include <cassert>
#include <iostream>
#include "EthernetLayer.h"


ssize_t EthernetLayer::send(void *buf, size_t size) {
    return 0;
}

void EthernetLayer::on_recv(void *buf, size_t size) {

    assert(size >= 64 && size <= 1518);

    auto* frame = (EthernetFrame*) buf;
    std::cout << "dst: " << frame->dest_mac.to_string() << " src: " << frame->src_mac.to_string() << std::endl;

}

EthernetLayer::EthernetLayer() = default;
