//
// Created by lyc8503 on 2022/10/20.
//

#include <iostream>
#include <cassert>
#include "ARP.h"

ssize_t ARP::send(void *buf, size_t size) {

}

void ARP::on_recv(void *buf, size_t size) {
    assert(size == 28);

    auto* payload = (ArpPayload*) buf;
    assert(payload->hlen == 6);
    assert(payload->plen == 4);



}
