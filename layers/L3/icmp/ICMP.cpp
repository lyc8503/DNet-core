//
// Created by lyc8503 on 2022/11/20.
//

#include "ICMP.h"
#include "../../../defs.h"
#include "../../../util/util.h"


void ICMP::on_recv(void *buf, size_t size) {

    auto* datagram = (ICMPDatagram*) buf;
    DNET_DEBUG("ICMP recv: %s", datagram->to_string().c_str());

    // Checksum
    uint16_t checksum = datagram->checksum.val();
    datagram->checksum = 0x0000;
    DNET_ASSERT(checksum_16bit((const uint16_be*) buf, size / 2) == checksum, "Checksum Mismatch.");

    switch (datagram->type) {
        case ICMP_TYPE::ECHO_REPLY:
            break;
        case ICMP_TYPE::ECHO_REQUEST:
            send(ICMP_TYPE::ECHO_REPLY, 0, (uint32_be) datagram->rest_of_header, datagram->data, size - sizeof(ICMPDatagram));
            break;
        default:
            DNET_ASSERT(false, "Unknown ICMP type.");
    }
}

void ICMP::send(uint8_t type, uint8_t code, uint32_be rest_of_header, uint8_t *data, size_t size) {
    char buf[sizeof(ICMPDatagram) + size];
    auto* datagram = (ICMPDatagram*) buf;

    datagram->type = type;
    datagram->code = code;
    datagram->rest_of_header = rest_of_header;
    memcpy(datagram->data, data, size);
    datagram->checksum = checksum_16bit((const uint16_be*) buf, size / 2);

    DNET_DEBUG("ICMP send: %s", datagram->to_string().c_str());

    // TODO
    DNET_ASSERT(false, "TODO");
}

ICMP::ICMP(DNet& context): context(context) {

}

std::ostream &operator<<(std::ostream &os, ICMP_TYPE op) {
    switch (op) {
        case ICMP_TYPE::ECHO_REQUEST:
            return os << "ECHO_REQUEST";
        case ICMP_TYPE::ECHO_REPLY:
            return os << "ECHO_REPLY";
        default:
            return os << "Unknown(" << (uint32_t) op << ")";
    }
}