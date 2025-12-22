#include "TCP.h"


TCP::TCP(DNet& context) : context(context) {}

void TCP::on_recv(void* buf, size_t size, L3Context l3_context) {
    auto* segment = (TcpSegment*) buf;

    TcpConnTuple tuple = {l3_context.src_ip, segment->src_port.val(), l3_context.dest_ip, segment->dest_port.val()};
    DNET_DEBUG("L4 TCP recv %s: %s", tuple.to_string().c_str(), segment->to_string().c_str());
    // TODO: validate checksum

    auto it = connections.find(tuple);
    if (it == connections.end()) {
        // No existing connection, it should be a SYN packet from a client to establish a new connection
        // TODO: improve this init
        if (!segment->flag_bits.syn) {
            DNET_DEBUG("TCP %s has no SYN flag nor existing connection, dropping", tuple.to_string().c_str());
            return;
        }

        DNET_DEBUG("TCP SYN %s creating new connection", tuple.to_string().c_str());
        std::shared_ptr<TcpConn> conn = std::make_shared<TcpConn>();
        conn->tuple = tuple;
        conn->state = SYN_RECVED;
        conn->rcv_nxt = segment->seq.val() + 1;
        conn->snd_una = rand();  // choose a random initial sequence number on server side
        conn->snd_nxt = conn->snd_una + 1;

        uint8_t rbuf[sizeof(TcpSegment)];
        auto* rsegment = (TcpSegment*) rbuf;
        memset(rbuf, 0, sizeof(TcpSegment));
        rsegment->src_port = uint16_be(tuple.dest_port);
        rsegment->dest_port = uint16_be(tuple.src_port);
        rsegment->seq = conn->snd_una;
        rsegment->ack = uint32_be(conn->rcv_nxt);  // Acknowledge the SYN
        rsegment->data_offset = (sizeof(TcpSegment) / 4) << 4;
        rsegment->flag_bits.syn = 1;
        rsegment->flag_bits.ack = 1;
        rsegment->window = uint16_be(WINDOW_SIZE);

        // TODO: better checksum method
        uint8_t pseudo_header_buf[sizeof(PseudoHeader) + sizeof(TcpSegment)];
        auto* pseudo_header = (PseudoHeader*) pseudo_header_buf;
        pseudo_header->src_ip = tuple.dest_ip;
        pseudo_header->dest_ip = tuple.src_ip;
        pseudo_header->zero = 0;
        pseudo_header->protocol = (uint8_t) IPV4_PROTOCOL::TCP;
        pseudo_header->length = uint16_be(sizeof(TcpSegment));
        memcpy(pseudo_header_buf + sizeof(PseudoHeader), rbuf, sizeof(TcpSegment));
        uint16_t checksum = checksum_16bit_be(pseudo_header_buf, sizeof(PseudoHeader) + sizeof(TcpSegment));
        rsegment->checksum = uint16_be(checksum);

        L3Context rcontext;
        rcontext.src_ip = tuple.dest_ip;
        rcontext.dest_ip = tuple.src_ip;
        rcontext.protocol = IPV4_PROTOCOL::TCP;
        DNET_DEBUG("L4 TCP send %s: %s", tuple.to_string().c_str(), rsegment->to_string().c_str());
        context.L4_send(rbuf, sizeof(TcpSegment), rcontext);
        connections.emplace(tuple, conn);
        DNET_DEBUG("TCP connection %s created LISTEN->SYN_RECVED, sent SYN-ACK", tuple.to_string().c_str());  // LISTEN by default on all ports
        return;
    }

    auto conn = it->second;
    switch (conn->state)
    {
        case TcpState::SYN_RECVED:  // step 3 of TCP three-way handshake
            if (segment->flag_bits.ack && segment->ack.val() == conn->snd_nxt) {
                DNET_DEBUG("TCP connection %s SYN_RECVED->ESTABLISHED", tuple.to_string().c_str());
                conn->state = TcpState::ESTABLISHED;
                // conn->snd_una = segment->ack.val();
                return;
            }
            DNET_DEBUG("TCP connection %s unexpected packet in SYN_RECVED state: %s", tuple.to_string().c_str(), segment->to_string().c_str());
            break;
        default:
            DNET_DEBUG("Unexpected TCP state %d, tuple %s, packet %s", conn->state, tuple.to_string().c_str(), segment->to_string().c_str());
            DNET_ASSERT(false, "unexpected TCP state");
            break;
    }

}
