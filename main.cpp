
#include <csignal>
#include <unistd.h>
#include "DNet.h"


int main(int argc, char** argv) {
    srand((unsigned int) time(nullptr));

    if (argc == 6) {
        DNet dnet(argv[1], static_cast<size_t>(std::stoul(argv[2])), argv[3], argv[4], argv[5]);
        // DNet dnet("dnet0", 1500, "11:45:14:19:19:81", "10.0.0.0", "255.255.255.0");
    } else {
        fprintf(stderr, "Usage: %s <ifname> <mtu> <mac> <dest_ip> <gen_mask>\n", argv[0]);
        return 1;
    }


    while (true) {
        sleep(10);
    }

    return 0;
}
