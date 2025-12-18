#include "TCP.h"

TCP::TCP(DNet& context) : context(context) {}

void TCP::on_recv(void* buf, size_t size, L3Context l3_context) {
    auto* segment = (TcpSegment*) buf;

    DNET_DEBUG("L4 TCP recv: %s", segment->to_string().c_str());

}
