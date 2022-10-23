//
// Created by lyc8503 on 2022/10/21.
//

#ifndef DNET_DNET_H
#define DNET_DNET_H

#include "driver/driver.h"
#include "layers/defs.h"
#include "layers/L2/L2.h"

class L2;


class DNet {

public:
    DNet(const std::string &ifname, int mtu, const std::string &dest_ip, const std::string &gen_mask);

    driver *dri;
    L2 *ethernet_layer;

    const MacAddress& mac();
    const Ipv4Subnet& subnet();

private:
    Ipv4Subnet ipv4_subnet;

    MacAddress mac_address;
};


#endif //DNET_DNET_H
