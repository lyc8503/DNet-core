// define custom DNS records here

#ifndef DNET_DNS_RECORD_H
#define DNET_DNS_RECORD_H

#include "DNS.h"

void register_dns_records(std::unordered_map<record_key, record_value>& dns_records) {
    REGISTER_A_RECORD("example.com", "1.2.3.4");
}

#endif  // DNET_DNS_RECORD_H