//
// Created by lyc8503 on 2022/10/21.
//

#ifndef DNET_DNET_H
#define DNET_DNET_H


#include "driver/driver.h"
#include "layers/defs.h"
#include "layers/L2/L2.h"
#include "layers/L3/L3.h"

class L2;
class L3;

class DNet {

public:
    DNet(const std::string &ifname, int mtu, const std::string &dest_ip, const std::string &gen_mask);

    ssize_t driver_send(void *buf, size_t size);

    ssize_t L2_send(void *buf, size_t size, MacAddress dest);

    void L3_on_recv(void* buf, size_t size);

    const MacAddress &mac();

    const Ipv4Subnet &subnet();

private:
    driver *dri;
    L2 *ethernet_layer;
    L3 *ip_layer;

    Ipv4Subnet ipv4_subnet;
    MacAddress mac_address;
};


#endif //DNET_DNET_H
