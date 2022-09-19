//
// Created by lyc8503 on 2022/9/18.
//

#ifndef DNET_DRIVER_H
#define DNET_DRIVER_H

#include <linux/if.h>
#include <linux/if_tun.h>
#include <cstdint>
#include <string>


// Refer to https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/networking/tuntap.rst?id=HEAD
class driver {

public:
    driver();
    explicit driver(const std::string& dev);
    bool init_dev();
    char dev[IFNAMSIZ]{};
    int fd = -1;

    ssize_t read(uint8_t* buf, size_t size);
    ssize_t write(uint8_t* buf, size_t size);

private:

};


#endif //DNET_DRIVER_H
