//
// Created by lyc8503 on 2022/9/18.
//

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "driver.h"
#include <sys/ioctl.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <net/route.h>
#include <sstream>
#include <iomanip>
#include "../defs.h"


driver::driver() = default;

driver::driver(const std::string& dev, size_t mtu) {
    strncpy(this->dev, dev.c_str(), IFNAMSIZ);
    this->mtu = mtu;
}

// init device for read on linux
bool driver::init_dev() {
    static_assert(IFHWADDRLEN == 6, "Hardware address length is not 6 on this platform.");

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

    // set device up
    int fd_;
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    if ((fd_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        DNET_ERROR("Failed to open socket on PF_INET.");
        return false;
    }
    if (ioctl(fd_, SIOCSIFFLAGS, &ifr) < 0) {
        DNET_ERROR("Could not ioctl set flag: %s", strerror(errno));
        close(fd_);
        return false;
    }

    // fetch hwaddr (mac)
    if (ioctl(fd_, SIOCGIFHWADDR, &ifr) < 0) {
        DNET_ERROR("Could not ioctl get hwaddr: %s", strerror(errno));
        close(fd_);
        return false;
    }
    close(fd_);

    memcpy(mac, &ifr.ifr_hwaddr, 6);
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

void driver::set_callback(std::function<void(void *, size_t)> callback) {
    this->callback = callback;
}

void driver::do_listen() {
    DNET_DEBUG("Driver do listen start.");
    while (do_listen_flag) {
        ssize_t ret;
        uint8_t data[this->mtu + 18];
        ret = this->read(data, this->mtu + 18);

        DNET_DEBUG("Driver payload recv: %ld bytes", ret);

        this->callback(data, ret);
    }
    DNET_DEBUG("Driver do listen exit.");
}

void driver::start_listen() {
    if (this->callback == nullptr) {
        throw std::logic_error("No callback set.");
    }
    do_listen_flag = true;
    this->thread = new std::thread(&driver::do_listen, this);
}

void driver::stop_listen() {
    do_listen_flag = false;
    delete this->thread;
    this->thread = nullptr;
}

// add a ip address to the TAP dev
bool driver::add_ip(const std::string& ip_addr, const std::string& net_mask) {
    int fd_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (fd_ < 0) {
        DNET_ERROR("Add ip socket open failed.");
        return false;
    }

    struct ifreq ifr{};
    strncpy(ifr.ifr_name, this->dev, IFNAMSIZ);
    ifr.ifr_addr.sa_family = AF_INET;

    auto* addr = (struct sockaddr_in*) &ifr.ifr_addr;
    if (inet_pton(AF_INET, ip_addr.c_str(), &addr->sin_addr) < 0) {
        goto error;
    }
    if (ioctl(fd_, SIOCSIFADDR, &ifr) < 0) {
        goto error;
    }

    if (inet_pton(AF_INET, net_mask.c_str(), &addr->sin_addr) < 0) {
        goto error;
    }
    if (ioctl(fd_, SIOCSIFNETMASK, &ifr) < 0) {
        goto error;
    }

    close(fd_);
    return true;

error:
    DNET_ERROR("Add ip failed: %s", strerror(errno));
    close(fd_);
    return false;
}

// add a route to the TAP dev
bool driver::add_route(const std::string& dest, const std::string& gen_mask) {

    int fd_ = socket( PF_INET, SOCK_DGRAM,  IPPROTO_IP);

    if (fd_ < 0) {
        DNET_ERROR("Add route socket open failed.");
        return false;
    }

    struct rtentry rt{};

    rt.rt_dev = this->dev;

    auto* addr = (struct sockaddr_in*) &rt.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(dest.c_str());

    addr = (struct sockaddr_in*) &rt.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(gen_mask.c_str());

    rt.rt_flags = RTF_UP;
    rt.rt_metric = 0;

    if (ioctl(fd_, SIOCADDRT, &rt) < 0) {
        DNET_ERROR("Add route failed: %s", strerror(errno));
        close(fd_);
        return false;
    }

    close(fd_);
    return true;
}

const uint8_t* driver::get_mac() {
    return mac;
}
