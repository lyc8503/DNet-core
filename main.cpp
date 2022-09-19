
#include "driver/driver.h"
#include "defs.h"


int main() {
    driver driver("dnet0");
    if (!driver.init_dev()) {
        return -1;
    }
    DNET_DEBUG("%s %d", driver.dev, driver.fd);

    // ip link set dev tap0 up
    // ip address add dev tap0 local 10.0.0.1
    // ip route add dev tap0 10.0.0.0/24

    while (true) {
        ssize_t t;
        uint8_t d[1024];
        t = driver.read(d, 1024);

        for (int i = 0; i < t; i++) {
            printf("%x ", d[i]);
        }
        printf("\n");
    }

    return 0;
}
