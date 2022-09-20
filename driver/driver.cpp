//
// Created by lyc8503 on 2022/9/18.
//

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "driver.h"
#include <sys/ioctl.h>

#include "../defs.h"



#define TEMP_MTU 1500

driver::driver() = default;

driver::driver(const std::string& dev) {
    strncpy(this->dev, dev.c_str(), IFNAMSIZ);
}

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
        DNET_ERROR("Failed to ip set link up.");
        return false;
    }

    // ip address add dev tap0 local 10.0.0.1
    // ip route add dev tap0 10.0.0.0/24
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

void driver::set_callback(std::function<void(void *, size_t)>& callback) {
    this->callback = &callback;
}

void driver::do_listen() {
    while (true) {
        ssize_t t;
        uint8_t d[TEMP_MTU];
        t = this->read(d, TEMP_MTU);

        DNET_DEBUG("TEST");

        (*this->callback)(d, t);
    }
}

void driver::start_listen() {
    this->thread = new std::thread(&driver::do_listen, this);
}

void driver::stop_listen() {

}
