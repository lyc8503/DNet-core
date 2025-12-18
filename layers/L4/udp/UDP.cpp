//
// Created by lyc8503 on 2022/12/11.
//

#include "UDP.h"

UDP::UDP(DNet &context) : context(context) {}

void UDP::on_recv(void *buf, size_t size, L3Context l3_context) {
    
    auto *datagram = (UdpDatagram *) buf;
    DNET_DEBUG("L4 recv: %s", datagram->to_string().c_str());

    DNET_ASSERT(datagram->length.val() == size, "Datagram length mismatch.");

    // TODO: Checksum validation (checksum for ipv4 UDP is optional)
//            if (datagram->checksum.val() != 0x0000) {
//                PseudoHeader pseudo_header;
//                pseudo_header.src_ip = l3_context.src_ip;
//                pseudo_header.dest_ip = l3_context.dest_ip;
//                pseudo_header.zero = 0x00;
//                pseudo_header.protocol = (uint8_t) IPV4_PROTOCOL::UDP;
//                pseudo_header.length = datagram->length;
//
//                uint16_t checksum = datagram->checksum.val();
//                datagram->checksum = 0x0000;
//
//                // TODO: optimize here
//                char tmp[size + sizeof(PseudoHeader)];
//                memcpy(tmp, &pseudo_header, sizeof(PseudoHeader));
//                memcpy(tmp + sizeof(PseudoHeader), datagram, size);
//
//                DNET_ASSERT(checksum_16bit_be(tmp, size + sizeof(PseudoHeader)) == checksum, "Checksum Mismatch.");
//            }

    // Echo (test here)
    L3Context con;
    con.protocol = IPV4_PROTOCOL::UDP;
    con.src_ip = l3_context.dest_ip;
    con.dest_ip = l3_context.src_ip;

    char reply[size];
    auto* reply_datagram = (UdpDatagram*) reply;
    reply_datagram->length = datagram->length;
    reply_datagram->dest_port = datagram->src_port;
    reply_datagram->src_port = datagram->dest_port;
    reply_datagram->checksum = 0x0000;  // TODO: calculate the checksum
    memcpy(reply_datagram->data, datagram->data, size - sizeof(UdpDatagram));

    DNET_DEBUG("L4 send: %s", reply_datagram->to_string().c_str());
    context.L4_send(reply_datagram, size, con);
}