//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include <cassert>
#include "ARP.h"
#include "../../../defs.h"

ssize_t ARP::send(void *buf, size_t size) {
    return 0;
}

void ARP::on_recv(void *buf, size_t size) {
    assert(size == 28);

    auto* payload = (ArpPayload*) buf;
    assert(payload->hlen == 6);  // Hardware address length (mac)
    assert(payload->plen == 4);  // Protocol address length (ipv4)

    std::cout << payload->to_string() << std::endl;

    switch (payload->opcode.val()) {
        case ARP_REQUEST:
            break;
        case ARP_RESPONSE:

            break;
        default:
            DNET_ERROR("Unknown ARP opcode: %d", payload->opcode.val());
    }

}
