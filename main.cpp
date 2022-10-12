
#include <csignal>
#include <cassert>
#include "driver/driver.h"
#include "ethernet/EthernetLayer.h"


void test_recv(void* buf, size_t size) {
    for (int i = 0; i < size; i++) {
        printf("%x ", ((uint8_t *)buf)[i]);
    }
    printf("\n");
}

int main() {
    driver driver("dnet0", 1500);

    assert(driver.init_dev());

//  We will manage ip by ourselves
//    driver.add_ip("10.0.0.1");

    assert(driver.add_route("10.0.0.0/24"));

    std::function<void(void *, size_t)> a(test_recv);
    driver.set_callback(&a);
    driver.start_listen();

//    sleep(1);
//    driver.stop_listen();

//    DNET_DEBUG("%s %d", driver.dev, driver.fd);

//    EthernetLayer l2(driver);



//    while (true) {
//        ssize_t t;
//        uint8_t d[1024];
//        t = driver.read(d, 1024);
//
//
//        printf("\n");
//    }

    while (1) {
        sleep(10);
    }

    return 0;
}
