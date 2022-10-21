//
// Created by lyc8503 on 2022/10/21.
//

#include <cassert>
#include "DNet.h"

void DNet::init() {
    dri = new class driver("dnet0", 1500);

    assert(dri->init_dev());
//  We will manage ip by ourselves
//    assert(driver.add_ip("10.0.0.1", "255.255.255.0"));

    assert(dri->add_route("10.0.0.0", "255.255.255.0"));

    ethernet_layer = new L2();

    std::function<void(void *, size_t)> L2_on_recv = [&](void* buf, size_t size) -> void {
        this->ethernet_layer->on_recv(buf, size);
    };

    dri->set_callback(L2_on_recv);
    dri->start_listen();

}
