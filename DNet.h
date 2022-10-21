//
// Created by lyc8503 on 2022/10/21.
//

#ifndef DNET_DNET_H
#define DNET_DNET_H


#include "layers/L2/L2.h"
#include "driver/driver.h"

class DNet {

public:
    driver* driver;
    L2* ethernet_layer;

    void init();

private:
    void L2_on_recv(void* buf, size_t size);
};


#endif //DNET_DNET_H
