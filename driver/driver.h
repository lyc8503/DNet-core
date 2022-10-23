//
// Created by lyc8503 on 2022/9/18.
//

#ifndef DNET_DRIVER_H
#define DNET_DRIVER_H

#include <linux/if.h>
#include <linux/if_tun.h>
#include <cstdint>
#include <string>
#include <thread>
#include <functional>
#include <atomic>


// Refer to https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/networking/tuntap.rst?id=HEAD
class driver {

public:
    driver();
    explicit driver(const std::string& dev, size_t mtu);
    bool init_dev();
    bool add_ip(const std::string& ip_addr, const std::string& netmask);
    bool add_route(const std::string& dest, const std::string& gen_mask);

    char dev[IFNAMSIZ]{};
    size_t mtu;

    ssize_t read(uint8_t* buf, size_t size);
    ssize_t write(uint8_t* buf, size_t size);

    void set_callback(std::function<void(void*, size_t)> callback);
    void start_listen();
    void stop_listen();

    const uint8_t* get_mac();

private:
    int fd = -1;
    std::thread* thread;
    std::function<void(void*, size_t)> callback;
    void do_listen();
    std::atomic_bool do_listen_flag;
    uint8_t mac[6];
};


#endif //DNET_DRIVER_H
