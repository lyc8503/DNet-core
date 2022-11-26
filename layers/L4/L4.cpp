//
// Created by lyc8503 on 2022/10/24.
//


#include "L4.h"
#include "../../defs.h"
#include "../../util/util.h"

L4::L4(DNet &context) : context(context) {

}

void L4::on_recv(void *buf, size_t size, L3Context l3_context) {
    switch (l3_context.protocol) {
        case IPV4_PROTOCOL::UDP: {
            auto *datagram = (UdpDatagram *) buf;
            DNET_DEBUG("L4 recv: %s", datagram->to_string().c_str());

            DNET_ASSERT(datagram->length.val() == size, "Datagram length mismatch.");

            // Checksum validation (checksum for ipv4 UDP is optional)
            if (datagram->checksum.val() != 0x0000) {
                PseudoHeader pseudo_header;
                pseudo_header.src_ip = l3_context.src_ip;
                pseudo_header.dest_ip = l3_context.dest_ip;
                pseudo_header.zero = 0x00;
                pseudo_header.protocol = (uint8_t) IPV4_PROTOCOL::UDP;
                pseudo_header.length = datagram->length;

                uint16_t checksum = datagram->checksum.val();
                datagram->checksum = 0x0000;

                // TODO: optimize here
                char tmp[size + sizeof(PseudoHeader)];
                memcpy(tmp, &pseudo_header, sizeof(PseudoHeader));
                memcpy(tmp + sizeof(PseudoHeader), datagram, size);

                DNET_ASSERT(checksum_16bit_be(tmp, size + sizeof(PseudoHeader)) == checksum, "Checksum Mismatch.");
            }

            break;
        }
        default:
            DNET_ASSERT(false, "Unknown ipv4 protocol: " + int_to_hex((uint8_t) l3_context.protocol));
    }
}

