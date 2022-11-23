
#include <csignal>
#include "DNet.h"


int main() {

    DNet dnet("dnet0", 1500, "192.168.1.55", "255.255.255.255");

    while (true) {
        sleep(10);
    }

    return 0;
}
