//
// Created by lyc8503 on 2022/10/24.
//

#ifndef DNET_L4_H
#define DNET_L4_H

#include <cstdint>
#include "../L3/L3.h"

enum class IPV4_PROTOCOL;

class L4 {
    
public:

    L4(DNet& context);

    void on_recv(void* buf, size_t size, IPV4_PROTOCOL protocol);    
    size_t send(void* buf, size_t size, IPV4_PROTOCOL protocol);

private:

    DNet& context;

};

#endif  // DNET_L4_H
