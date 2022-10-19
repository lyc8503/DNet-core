//
// Created by lyc8503 on 2022/9/20.
//

#ifndef DNET_LAYER_H
#define DNET_LAYER_H


#include <cstdio>

class Layer {
public:
    virtual ssize_t send(void* buf, size_t size) = 0;
    virtual void on_recv(void* buf, size_t size) = 0;
};


#endif //DNET_LAYER_H
