#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

class StatReader
{
public:
    StatReader(const StatReader&) = delete;
    StatReader& operator=(const StatReader&) = delete;
    StatReader(StatReader&&) = delete;
    StatReader& operator=(StatReader&&) = delete;
    ~StatReader() = default;
    StatReader() = default;

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;
    using HasherStop = TransportCatalogue::HasherStop;

    void PrintBus(TransportCatalogue& tc, std::string_view name)
    {
        auto bus = tc.GetBus(name);
        if (bus.busStops.empty()) {
            std::cout << "Bus " << name << ": " << "not found" << std::endl;
            return;
        }

        auto uniqCalc = [](const auto& stops) ->size_t {
            std::unordered_set<Stop, HasherStop> uniqueStops;
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
        std::cout << "Bus " << bus.name << ": "
                  << bus.busStops.size() << " stops on route, "
                  << uniqCalc(bus.busStops) << " unique stops, "
                  << std::setprecision(6) << distanceCalc << " route length"<< std::endl;
    }
};

void inline ProcessRequest(TransportCatalogue& tc, InputReader& ir, InputReadParser& parser, StatReader& sr)
{
    std::string line{};
    std::vector<std::string> busDataLines{};
    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        parser.ltrim(line);
        parser.rtrim(line);
        if (line.length() <= 0) {
            continue;
        }
        if (line.find("Bus") == 0)
        {
            sr.PrintBus(tc, line.substr(4, line.size() - 4));
        }
    } // end while
}
