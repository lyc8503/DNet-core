
#include <csignal>
#include "DNet.h"


int main() {

    DNet dnet("dnet0", 1500, "10.0.0.0", "255.255.255.0");


    while (true) {
        sleep(10);
    }

    return 0;
}
