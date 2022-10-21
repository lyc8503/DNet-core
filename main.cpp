
#include <csignal>
#include "DNet.h"


int main() {

    DNet dnet;

    dnet.init();

    while (true) {
        sleep(10);
    }

    return 0;
}
