#include "mdns-lookup.h"
#include <mdns.h>
#include "esp32-hal-log.h"

static const char *TAG = "MDNSLookup";

std::string MDNSLookup::lookup(const std::string& hostname) {
    ESP_ERROR_CHECK(mdns_init());

    const char *hostnameCstr = hostname.c_str();
    ESP_LOGI(TAG, "Query A: %s.local", hostnameCstr);

    struct ip4_addr addr{
            .addr = 0
    };

    ESP_ERROR_CHECK(mdns_query_a(hostnameCstr, 2000, &addr));

    ESP_LOGI(TAG, "Query A: %s.local resolved to: " IPSTR, hostnameCstr, IP2STR(&addr));

    const char *ipAddressChar = ip4addr_ntoa(&addr);
    return std::string(ipAddressChar);
};
