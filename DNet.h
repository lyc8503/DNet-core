//
// Created by lyc8503 on 2022/10/21.
//

#ifndef DNET_DNET_H
#define DNET_DNET_H


#include "driver/driver.h"
#include "layers/defs.h"
#include "layers/L2/L2.h"
#include "layers/L3/L3.h"
#include "layers/L4/L4.h"
#include <optional>

class L2;
class L3;
class L4;

enum class EtherType;
enum class IPV4_PROTOCOL;

struct L3Context;

class DNet {

public:
    DNet(const std::string &ifname, int mtu, const std::string &dest_ip, const std::string &gen_mask);

    ssize_t driver_send(void *buf, size_t size);

    ssize_t L2_send(void *buf, size_t size, MacAddress dest, EtherType type);

    ssize_t L3_send(Ipv4Address src, Ipv4Address target, IPV4_PROTOCOL protocol, void* buf, size_t size);

    ssize_t L4_send(void*buf, size_t size);
    
    void L4_on_recv(void* buf, size_t size, L3Context l3_context);

    void L3_on_recv(void* buf, size_t size);

    std::optional<MacAddress> arp_lookup(Ipv4Address address);

    const MacAddress &mac();

    const Ipv4Subnet &subnet();

private:
    driver *dri;
    L2 *ethernet_layer;
    L3 *ip_layer;
    L4 *transport_layer;

    Ipv4Subnet ipv4_subnet;
    MacAddress mac_address;
};


#endif //DNET_DNET_H
