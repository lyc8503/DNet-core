//
// Created by lyc8503 on 2022/12/11.
//

#include "UDP.h"
#include "../../L7/DNS.h"

UDP::UDP(DNet &context) : context(context) {
    dns = new DNS(context);
}

void UDP::on_recv(void *buf, size_t size, L3Context l3_context) {
    auto *datagram = (UdpDatagram *) buf;
    DNET_DEBUG("L4 UDP recv: %s", datagram->to_string().c_str());

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
        
        uint16_t expected = checksum_16bit_be(tmp, size + sizeof(PseudoHeader));
        if (expected != checksum) {
            DNET_DEBUG("UDP checksum mismatch, expected: %x, got: %x", expected, checksum);
        }
        DNET_ASSERT(expected == checksum, "Checksum mismatch");
    }

    if (datagram->dest_port.val() == 53) {
        // DNS
        dns->on_recv(datagram->data, size - sizeof(UdpDatagram), l3_context.src_ip, datagram->src_port.val(), l3_context.dest_ip, datagram->dest_port.val());
        return;
    }

    // ToUpper & Echo (test here)
    uint8_t reply_data[size - sizeof(UdpDatagram)];
    memcpy(reply_data, datagram->data, size - sizeof(UdpDatagram));
    for (size_t i = 0; i < size - sizeof(UdpDatagram); i++) {
        if (reply_data[i] >= 'a' && reply_data[i] <= 'z') {
            reply_data[i] += 'A' - 'a';
        }
    }

    send(reply_data, size - sizeof(UdpDatagram), l3_context.src_ip, datagram->src_port.val(), l3_context.dest_ip, datagram->dest_port.val());
}

ssize_t UDP::send(void* buf, size_t size, Ipv4Address dest, uint16_t dest_port, Ipv4Address src, uint16_t src_port) {
    size_t buf_len = size + sizeof(UdpDatagram);
    uint8_t datagram_buf[buf_len];
    auto* datagram = (UdpDatagram*) datagram_buf;
    datagram->src_port = src_port;
    datagram->dest_port = dest_port;
    datagram->length = buf_len;
    datagram->checksum = 0x0000;  // UDP checksum is optional for IPv4, set to 0
    memcpy(datagram->data, buf, size);

    DNET_DEBUG("L4 UDP send: %s", datagram->to_string().c_str());
    L3Context l3_context{
        .src_ip = src,
        .dest_ip = dest,
        .protocol = IPV4_PROTOCOL::UDP
    };
    return context.L4_send(datagram, buf_len, l3_context);
}