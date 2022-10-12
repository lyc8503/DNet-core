//
// Created by lyc8503 on 2022/9/18.
//

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "driver.h"
#include <sys/ioctl.h>
#include <stdexcept>

#include "../defs.h"


driver::driver() = default;

driver::driver(const std::string& dev, size_t mtu) {
    strncpy(this->dev, dev.c_str(), IFNAMSIZ);
    this->mtu = mtu;
}

// init device for read on linux
bool driver::init_dev() {
    struct ifreq ifr{};

    if ((fd = open("/dev/net/tap", O_RDWR)) < 0) {
        DNET_ERROR("Failed to open TAP device, check permissions or create one with `mknod /dev/net/tap c 10 200`.");
        return false;
    }

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, this->dev, IFNAMSIZ);

    if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
        DNET_ERROR("Could not ioctl tun: %s", strerror(errno));
        close(fd);
        return false;
    }

    /* if the operation was successful, write back the name of the
     * interface to the variable "dev". */
    strcpy(this->dev, ifr.ifr_name);

    // ip link set dev tap0 up
    int ret = system(("ip link set dev " + std::string(this->dev) + " up").c_str());
    if (ret) {
        DNET_ERROR("Failed to ip set link up: %d", ret);
        return false;
    }

    return true;
}


ssize_t driver::read(uint8_t *buf, size_t size) {
    ssize_t ret = ::read(this->fd, buf, size);
    if (ret < 0) {
        DNET_ERROR("Device read error: %s", strerror(errno));
    }
    return ret;
}

ssize_t driver::write(uint8_t *buf, size_t size) {
    ssize_t ret = ::write(this->fd, buf, size);
    if (ret < 0) {
        DNET_ERROR("Device write error: %s", strerror(errno));
    }
    return ret;
}

void driver::set_callback(std::function<void(void *, size_t)>* callback) {
    this->callback = callback;
}

void driver::do_listen() {
    DNET_DEBUG("Driver do listen start.");
    while (do_listen_flag) {
        ssize_t ret;
        uint8_t data[this->mtu];
        ret = this->read(data, this->mtu);

        DNET_DEBUG("Driver payload recv: %ld bytes", ret);

        (*this->callback)(data, ret);
    }
    DNET_DEBUG("Driver do listen exit.");
}

void driver::start_listen() {
    if (this->thread == nullptr) {
        throw std::logic_error("No callback set.");
    }
    do_listen_flag = true;
    this->thread = new std::thread(&driver::do_listen, this);
}

void driver::stop_listen() {
    do_listen_flag = false;
    this->thread = nullptr;
}

bool driver::add_ip(const std::string& ip_addr) {
    // The arg `ip_addr` is not checked!
    int ret = system(("ip address add dev " + std::string(this->dev) + " local " + ip_addr).c_str());
    if (ret) {
        DNET_ERROR("Add ip failed: %d", ret);
    }
    return ret == 0;
}

bool driver::add_route(const std::string& route) {
    // The arg `route` is not checked!
    int ret = system(("ip route add dev " + std::string(this->dev) + " " + route).c_str());
    if (ret) {
        DNET_ERROR("Add route failed: %d", ret);
    }
    return ret == 0;
}
