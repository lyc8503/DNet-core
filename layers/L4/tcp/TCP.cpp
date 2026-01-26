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

        DNET_DEBUG("TCP SYN %s creating new LISTEN connection", tuple.to_string().c_str());
        std::shared_ptr<TcpConn> conn = std::make_shared<TcpConn>();
        conn->tuple = tuple;
        conn->state = LISTEN;
        connections.emplace(tuple, conn);
    }

    it = connections.find(tuple);
    DNET_ASSERT(it != connections.end(), "TCP connection should exist");
    auto conn = it->second;

    L3Context rcontext;
    rcontext.src_ip = tuple.dest_ip;
    rcontext.dest_ip = tuple.src_ip;
    rcontext.protocol = IPV4_PROTOCOL::TCP;

    uint8_t rbuf[sizeof(TcpSegment) + 1500];
    auto* rsegment = (TcpSegment*) rbuf;
    memset(rbuf, 0, sizeof(TcpSegment));
    rsegment->src_port = uint16_be(tuple.dest_port);
    rsegment->dest_port = uint16_be(tuple.src_port);

    switch (conn->state)
    {
        case TcpState::LISTEN:
            // DNET_DEBUG("TCP connection %s LISTEN->SYN_RECVED", tuple.to_string().c_str());
            conn->state = TcpState::SYN_RECVED;
            conn->rcv_nxt = segment->seq.val() + 1;  // received SYN consumes one sequence number
            conn->snd_una = rand();  // choose a random initial sequence number on server side
            conn->snd_nxt = conn->snd_una + 1;  // SYN-ACK to be sent will consume one sequence number

            rsegment->seq = conn->snd_una;
            rsegment->ack = uint32_be(conn->rcv_nxt);  // Acknowledge the SYN
            rsegment->data_offset = (sizeof(TcpSegment) / 4) << 4;
            rsegment->flag_bits.syn = 1;
            rsegment->flag_bits.ack = 1;
            rsegment->window = uint16_be(WINDOW_SIZE);
            // TODO: better checksum method
            rsegment->checksum = TCP_CHECKSUM(rsegment, tuple.dest_ip, tuple.src_ip, nullptr, 0);

            DNET_DEBUG("L4 TCP send %s: %s", tuple.to_string().c_str(), rsegment->to_string().c_str());
            context.L4_send(rbuf, sizeof(TcpSegment), rcontext);
            DNET_DEBUG("TCP connection %s LISTEN->SYN_RECVED, sent SYN-ACK", tuple.to_string().c_str());  // LISTEN by default on all ports
            break;

        case TcpState::SYN_RECVED:  // step 3 of TCP three-way handshake
            if (segment->flag_bits.ack && segment->ack.val() == conn->snd_nxt) {
                // peer acknowledged our SYN-ACK
                DNET_DEBUG("TCP connection %s SYN_RECVED->ESTABLISHED", tuple.to_string().c_str());
                conn->state = TcpState::ESTABLISHED;
                conn->snd_una = segment->ack.val();
                return;
            }
            DNET_DEBUG("TCP connection %s unexpected packet in SYN_RECVED state: %s", tuple.to_string().c_str(), segment->to_string().c_str());
            break;
        case TcpState::ESTABLISHED:
            DNET_DEBUG("TCP connection %s ESTABLISHED received packet: %s", tuple.to_string().c_str(), segment->to_string().c_str());
            
            if (segment->seq.val() == conn->rcv_nxt) {
                // In-order Incoming packet
                uint16_t header_size = (segment->data_offset >> 4) * 4;
                uint16_t payload_size = size - header_size;
                DNET_DEBUG("TCP connection %s received in-order packet with payload size %d", tuple.to_string().c_str(), payload_size);

                DNET_DEBUG("TCP connection %s received payload: %s", tuple.to_string().c_str(), std::string((char*)buf + header_size, payload_size).c_str());
                // DROP THE PAYLOAD
                
                conn->rcv_nxt = segment->seq.val() + payload_size;

                // Send ACK
                rsegment->seq = conn->snd_nxt;
                rsegment->ack = uint32_be(conn->rcv_nxt);
                rsegment->data_offset = (sizeof(TcpSegment) / 4) << 4;
                rsegment->flag_bits.ack = 1;
                rsegment->window = uint16_be(WINDOW_SIZE);
                memcpy(rbuf + sizeof(TcpSegment), buf + header_size, payload_size);  // echo back the payload (for testing)  
                for (uint16_t i = 0; i < payload_size; i++) {
                    // to upper case
                    if (rbuf[sizeof(TcpSegment) + i] >= 'a' && rbuf[sizeof(TcpSegment) + i] <= 'z') {
                        rbuf[sizeof(TcpSegment) + i] = rbuf[sizeof(TcpSegment) + i] - 'a' + 'A';
                    }
                }
                rsegment->checksum = TCP_CHECKSUM(rsegment, tuple.dest_ip, tuple.src_ip, rbuf + sizeof(TcpSegment), payload_size);
                conn->snd_nxt += payload_size;

                DNET_DEBUG("L4 TCP send %s: %s", tuple.to_string().c_str(), rsegment->to_string().c_str());
                context.L4_send(rbuf, sizeof(TcpSegment) + payload_size, rcontext);
                DNET_DEBUG("TCP connection %s sent ACK for received data", tuple.to_string().c_str());
            }
            break;
        default:
            DNET_DEBUG("Unexpected TCP state %d, tuple %s, packet %s", conn->state, tuple.to_string().c_str(), segment->to_string().c_str());
            DNET_ASSERT(false, "unexpected TCP state");
            break;
    }
}
