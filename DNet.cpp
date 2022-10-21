//
// Created by lyc8503 on 2022/10/21.
//

#include <cassert>
#include "DNet.h"

void DNet::init() {
    driver = new class driver("dnet0", 1500);


    assert(driver->init_dev());
//  We will manage ip by ourselves
//    assert(driver.add_ip("10.0.0.1", "255.255.255.0"));

    assert(driver->add_route("10.0.0.0", "255.255.255.0"));


    ethernet_layer = new L2();

//    std::function<void(void *, size_t)> *callback(&this->L2_on_recv);
//    driver->set_callback(callback);
//    driver->start_listen();

}

void DNet::L2_on_recv(void *buf, size_t size) {
    this->ethernet_layer->on_recv(buf, size);
}
