#pragma once

#include <string>
#include <iostream>
#include <string_view>
#include "transport_catalogue.h"

namespace tc {
namespace io {
class InputReader
{
    std::istream& is_;
public:
    InputReader(std::istream& is) : is_{is} {}
    InputReader(const InputReader&) = delete;
    InputReader& operator=(const InputReader&) = delete;
    InputReader(InputReader&&) = delete;
    InputReader& operator=(InputReader&&) = delete;
    ~InputReader() = default;

    [[nodiscard]] std::string ReadLine() const;
    [[nodiscard]] int ReadLineWithNumber() const;
};

class InputReadParser
{
public:
    InputReadParser() = default;
    InputReadParser(const InputReadParser&) = delete;
    InputReadParser& operator=(const InputReadParser&) = delete;
    InputReadParser(InputReadParser&&) = delete;
    InputReadParser& operator=(InputReadParser&&) = delete;
    ~InputReadParser() = default;

    void ltrim(std::string& s);
    void rtrim(std::string& s);

    [[nodiscard]] TransportCatalogue::RetParseBus ParseBus(std::string_view busDataLine);
    [[nodiscard]] TransportCatalogue::RetParseStop ParseStop(std::string& line);
    [[nodiscard]] TransportCatalogue::RetParseDistancesBetween ParseDistancesBetween(std::string_view line);
};

[[nodiscard]] TransportCatalogue Load(const InputReader& ir);

} // namespace io
} // namespace tc