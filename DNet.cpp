//
// Created by lyc8503 on 2022/10/21.
//

#include <cstring>
#include "DNet.h"

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#error "Not supported on big endian architecture."
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
// Ok!
#else
#error "Unknown architecture!"
#endif

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

//    We will manage ip by ourselves
//    driver.add_ip("10.0.0.1", "255.255.255.0");
    if (!dri->add_route(this->ipv4_subnet.network.to_string(), this->ipv4_subnet.mask.to_string())) {
        throw std::runtime_error("Driver add route failed.");
    }

    /**
     * initialize ethernet layer
     */
    ethernet_layer = new class L2(*this);

    std::function<void(void *, size_t)> L2_on_recv = [&](void *buf, size_t size) -> void {
        this->ethernet_layer->on_recv(buf, size);
    };

    dri->set_callback(L2_on_recv);

    /**
     * initialize IP layer
     */

    ip_layer = new class L3(*this);



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

ssize_t DNet::driver_send(void* buf, size_t size) {
    return dri->write(buf, size);
}

ssize_t DNet::L2_send(void *buf, size_t size, MacAddress dest) {
    return this->ethernet_layer->send(buf, size, dest);
}

void DNet::L3_on_recv(void *buf, size_t size) {
    ip_layer->on_recv(buf, size);
}


