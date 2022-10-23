//
// Created by lyc8503 on 2022/10/21.
//

#include <cstring>
#include "DNet.h"

DNet::DNet(const std::string &ifname, int mtu, const std::string &dest_ip, const std::string &gen_mask) {
    /**
     * initialize args
     */
    this->ipv4_subnet.network.parse_string(dest_ip);
    this->ipv4_subnet.mask.parse_string(gen_mask);

    /**
     * initialize driver
     */
    dri = new class driver(ifname, mtu);
    if (!dri->init_dev()) {
        throw std::runtime_error("Driver init failed.");
    }

//  We will manage ip by ourselves
//  assert(driver.add_ip("10.0.0.1", "255.255.255.0"));
    if (!dri->add_route(this->ipv4_subnet.network.to_string(), this->ipv4_subnet.mask.to_string())) {
        throw std::runtime_error("Driver add route failed.");
    }

    /**
     * initialize ethernet layer
     */
    ethernet_layer = new class L2(this);

    std::function<void(void *, size_t)> L2_on_recv = [&](void *buf, size_t size) -> void {
        this->ethernet_layer->on_recv(buf, size);
    };

    dri->set_callback(L2_on_recv);



    /**
     * all layers are initialized, start!
     */
    dri->start_listen();
}

const MacAddress &DNet::mac() {
    memcpy(this->mac_address.bytes, this->dri->get_mac(), IFHWADDRLEN);
    return this->mac_address;
}

const Ipv4Subnet &DNet::subnet() {
    return this->ipv4_subnet;
}
