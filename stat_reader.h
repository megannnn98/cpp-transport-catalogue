#pragma once

#include "transport_catalogue.h"
#include "input_reader.h"
#include <ostream>

namespace tc {
namespace io {
class StatReader
{
    std::ostream& os_;
public:
    StatReader(const StatReader&) = delete;
    StatReader& operator=(const StatReader&) = delete;
    StatReader(StatReader&&) = delete;
    StatReader& operator=(StatReader&&) = delete;
    ~StatReader() = default;
    StatReader(std::ostream& os) : os_{os} {}

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;

    void PrintBus(const TransportCatalogue& tc, std::string_view name) const;
    void PrintStop(const TransportCatalogue& tc, std::string_view name) const;
};

void ProcessRequest(const TransportCatalogue& tc, const InputReader& ir, const StatReader& sr);
} // namespace io
} // namespace tc