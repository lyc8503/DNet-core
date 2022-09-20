
#include "driver/driver.h"
#include "defs.h"
#include "ethernet/EthernetLayer.h"


void test_recv(void* buf, size_t size) {
    printf("%x", ((char*)buf)[0]);
}

int main() {
    driver driver("dnet0");
    if (!driver.init_dev()) {
        return -1;
    }

    std::function<void(void *, size_t)> a(test_recv);

    driver.set_callback(a);
    driver.start_listen();
//    DNET_DEBUG("%s %d", driver.dev, driver.fd);


//    EthernetLayer l2(driver);



//    while (true) {
//        ssize_t t;
//        uint8_t d[1024];
//        t = driver.read(d, 1024);
//
//        for (int i = 0; i < t; i++) {
//            printf("%x ", d[i]);
//        }
//        printf("\n");
//    }

    while(1);

    return 0;
}
