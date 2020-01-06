#ifndef TEMPERATURE_SENSOR_MDNS_LOOKUP_H
#define TEMPERATURE_SENSOR_MDNS_LOOKUP_H

#include <string>

class MDNSLookup {
public:
    /// Use MDNS to lookup hostname and return dot formatted ipv4 address
    /// \param hostname which exists in .local domain
    /// \return IP address in format XXX.XXX.XXX.XXX
    static std::string lookup(const std::string& hostname);
};

#endif //TEMPERATURE_SENSOR_MDNS_LOOKUP_H
