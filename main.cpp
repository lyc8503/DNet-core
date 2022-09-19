
#include "driver/driver.h"
#include "defs.h"


int main() {
    driver driver;
    driver.init_dev();
    DNET_DEBUG("%s %d", driver.dev, driver.fd);

    return 0;
}
