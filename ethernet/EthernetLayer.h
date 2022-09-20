//
// Created by lyc8503 on 2022/9/20.
//

#ifndef DNET_ETHERNETLAYER_H
#define DNET_ETHERNETLAYER_H

#include "../Layer.h"
#include "../driver/driver.h"


class EthernetLayer: Layer{

public:
    EthernetLayer(driver dri);

    virtual ssize_t send(void* buf, size_t size) = 0;
//    virtual ssize_t on_recv(void* buf, size_t size) = 0;

};


#endif //DNET_ETHERNETLAYER_H
