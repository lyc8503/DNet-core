//
// Created by lyc8503 on 2022/10/21.
//

#ifndef DNET_DNET_H
#define DNET_DNET_H


#include "layers/L2/L2.h"
#include "driver/driver.h"

class DNet {

public:
    driver* dri;
    L2* ethernet_layer;

    void init();

};


#endif //DNET_DNET_H
