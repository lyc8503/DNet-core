// define custom DNS records here

#ifndef DNET_DNS_RECORD_H
#define DNET_DNS_RECORD_H

#include "DNS.h"

void register_dns_records(std::unordered_multimap<record_key, record_value>& dns_records) {
    REGISTER_GENERIC_RECORD(TYPE_SOA, "example.com",
        28,
        "\x03ns1\xc0\x0c"  // MNAME: pointer to example.com
        "\xc0\x0c"  // RNAME: pointer to example.com
        "\x00\x00\x00\x01"  // SERIAL: 1
        "\x00\x00\x27\x10"  // REFRESH: 10000
        "\x00\x00\x09\x60"  // RETRY: 2400
        "\x00\x01\x51\x80"  // EXPIRE: 86400
        "\x00\x00\x07\x08"  // MINIMUM: 1800
    );

    REGISTER_A_RECORD("example.com", "1.2.3.4");
    REGISTER_GENERIC_RECORD(TYPE_AAAA, "example.com", 16, "\x20\x01\x0d\xb8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01");
    REGISTER_GENERIC_RECORD(TYPE_NS, "example.com",
        6,
        "\x03""ns1""\xc0\x0c"
    );
    REGISTER_GENERIC_RECORD(TYPE_NS, "example.com",
        6,
        "\x03""ns2""\xc0\x0c"
    );
    REGISTER_GENERIC_RECORD(TYPE_CNAME, "www.example.com",
        18,
        "\x04""test""\x07""example""\x03""com""\x00"
    );
}

#endif  // DNET_DNS_RECORD_H