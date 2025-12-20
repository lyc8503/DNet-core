#include "DNS.h"
#include "DNSRecord.h"

DNS::DNS(DNet& dnet): dnet(dnet) {
    register_dns_records(dns_records);
}

void DNS::on_recv(void* buf, size_t size, Ipv4Address src_addr, uint16_t src_port, Ipv4Address dest_addr, uint16_t dest_port) {
    auto* packet = (DNSPacket*) buf;
    DNET_DEBUG("DNS packet recv: %s", packet->header.to_string().c_str());

    DNET_ASSERT(packet->header.qr == 0, "DNS response packet received, ignored");

    // RFC9619: In the DNS, QDCOUNT Is (Usually) One
    // HOORAY! I don't bother to implement multiple questions parsing then
    DNET_ASSERT(packet->header.qdcount.val() == 1, "DNS packet with qdcount != 1 received, not supported");

    // parse question
    // I know I should do ton of boundary checks here, but it's a pain to implement it now
    char query_name[512];
    size_t ptr = 0;

    uint8_t* data_ptr = (uint8_t*) packet->data;
    while (*data_ptr != 0) {
        memcpy(query_name + ptr, data_ptr + 1, *data_ptr);
        ptr += *data_ptr;
        data_ptr += *data_ptr + 1;
        query_name[ptr++] = '.';
    }
    query_name[ptr - 1] = '\0';  // replace last '.' with '\0'

    uint16_be qtype = *(uint16_be*)(data_ptr + 1);
    uint16_be qclass = *(uint16_be*)(data_ptr + 3);

    DNET_DEBUG("DNS query for name: %s, qtype: %d, qclass: %d", query_name, qtype.val(), qclass.val());
    DNET_ASSERT(qclass == CLASS_IN, "DNS query with non-IN class received, WTF");

    // lowercase query_name
    for (size_t i = 0; query_name[i] != '\0'; i++) {
        if (query_name[i] >= 'A' && query_name[i] <= 'Z') {
            query_name[i] = query_name[i] - ('A' - 'a');
        }
    }

    auto response_iter = dns_records.find(std::make_tuple(qtype.val(), std::string(query_name)));
    if (response_iter == dns_records.end()) {
        // Actually this is a documented bad case in RFC4074, TODO: return empty ans for AAAA instead of NXDOMAIN
        DNET_DEBUG("No DNS record found for query, sending NXDOMAIN");

        size_t response_size = size;
        uint8_t response_buf[512];
        memcpy(response_buf, buf, size);
        auto header = (DNSHeader*) response_buf;
        header->qr = 1;  // response
        header->rcode = 3;  // NXDOMAIN

        dnet.UDP_send(response_buf, response_size, src_addr, src_port, dest_addr, dest_port);
        return;
    }

    auto response = response_iter->second;
    DNET_DEBUG("DNS record found, length: %d", std::get<0>(response_iter->second));

    // construct response packet
    size_t response_size = size + sizeof(DNSResourceRecord) + std::get<0>(response);  // record header + rdata
    uint8_t response_buf[512];
    memcpy(response_buf, buf, size);
    auto header = (DNSHeader*) response_buf;
    header->qr = 1;  // response
    header->aa = DNS_AUTHORITATIVE_ANSWER;  // authoritative answer
    header->ancount = 0;

    auto resource = (DNSResourceRecord*)(response_buf + size);
    resource->name = 0xc00c;  // pointer to domain name in question
    resource->type = qtype;
    resource->class_ = qclass;
    resource->ttl = DNS_DEFAULT_TTL;
    resource->rdlength = (uint16_be) std::get<0>(response);
    memcpy(resource->rdata, std::get<1>(response), std::get<0>(response));

    dnet.UDP_send(response_buf, response_size, src_addr, src_port, dest_addr, dest_port);
}