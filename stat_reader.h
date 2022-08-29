#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <ostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

namespace output {
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
        if (bus.busStops.empty()) {
            os_ << "Bus " << name << ": " << "not found" << std::endl;
            return;
        }

        auto uniqCalc = [](const auto& stops) ->size_t {
            std::unordered_set<Stop, Stop::Hasher> uniqueStops;
            for (std::size_t i{}; i < stops.size(); ++i) {
                uniqueStops.insert(*stops[i]);
            }
            return uniqueStops.size();
        };

        const double distanceCalc = std::transform_reduce(bus.busStops.begin(),
                             bus.busStops.end() - 1,
                             bus.busStops.begin() + 1,
                             std::size_t(0),
                             std::plus<std::size_t>(),
                             [](const Stop* l, const Stop* r){
           return ComputeDistance(l->coord, r->coord);
        });


        // Bus X: R stops on route, U unique stops, L route length
        os_ << "Bus " << bus.name << ": "
                  << bus.busStops.size() << " stops on route, "
                  << uniqCalc(bus.busStops) << " unique stops, "
                  << std::setprecision(6) << distanceCalc << " route length"<< std::endl;
    }
};

void inline ProcessRequest(TransportCatalogue& tc, input::InputReader& ir, input::InputReadParser& parser, StatReader& sr)
{
    using namespace std::literals;
    static constexpr std::string_view BUS = "Bus"sv;
    std::string line{};
    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        parser.ltrim(line);
        parser.rtrim(line);
        if (line.length() <= 0) {
            continue;
        }
        if (line.find(BUS.data()) == 0)
        {
            sr.PrintBus(tc, line.substr(BUS.size() + 1, line.size() - (BUS.size() + 1)));
        }
    } // end while
    std::cout << std::endl;
}
} // namespace output
