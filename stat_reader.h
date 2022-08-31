#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <ostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

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

    void PrintBus(TransportCatalogue& tc, std::string_view name) const
    {
        auto bus = tc.GetBus(name);
        auto stops = tc.GetBusStops(name);
        if (stops.empty()) {
            os_ << "Bus " << name << ": " << "not found" << std::endl;
            return;
        }

        auto uniqCalc = [](const std::vector<std::string_view>& stops) ->size_t {
            std::unordered_set<std::string_view> uniqueStops{};
            std::for_each(stops.cbegin(),
                          stops.cend(),
                          [&uniqueStops](const std::string_view stop){
                uniqueStops.insert(stop);
            });
            return uniqueStops.size();
        };

        double distance{};
        std::vector<std::string_view>::iterator it = stops.begin();
        while (it != (stops.end() - 1)) {
            auto coords1 = tc.GetStopCoords(*it);
            auto coords2 = tc.GetStopCoords(*(std::next(it)));
            distance += ComputeDistance(coords1, coords2);
            it = std::next(it);
        }

        // Bus X: R stops on route, U unique stops, L route length
        os_ << "Bus " << bus.name << ": "
                  << stops.size() << " stops on route, "
                  << uniqCalc(stops) << " unique stops, "
                  << std::setprecision(6) << distance << " route length"<< std::endl;
    }
};

void inline ProcessRequest(TransportCatalogue& tc, const InputReader& ir, const StatReader& sr)
{
    using namespace std::literals;
    static constexpr std::string_view BUS = "Bus"sv;
    InputReadParser parser{};
    std::string line{};
    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        parser.ltrim(line);
        parser.rtrim(line);
        if (line.length() && !line.find(BUS.data()))
        {
            sr.PrintBus(tc, line.substr(BUS.size() + 1, line.size() - (BUS.size() + 1)));
        }
    } // end while
    std::cout << std::endl;
}
} // namespace io
