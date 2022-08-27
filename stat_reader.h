#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include <iostream>
#include <vector>
#include <algorithm>

class StatReader
{
public:
    StatReader() = default;
    StatReader(const StatReader&) = default;
    StatReader& operator=(const StatReader&) = default;
    StatReader(StatReader&&) = default;
    StatReader& operator=(StatReader&&) = default;
    ~StatReader() = default;

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;

    void PrintBus(const Bus& bus)
    {
        // Bus X: R stops on route, U unique stops, L route length

        std::vector<std::string> uniqueStops{bus.busStops.begin(), bus.busStops.end()};
        uniqueStops.erase(std::unique(uniqueStops.begin(),
                                      uniqueStops.end()), uniqueStops.end());


//      double ComputeDistance(Coordinates from, Coordinates to)
//      std::vector<std::string> busStops;
        double distance{};
        auto it = bus.busStops.begin();
        auto last = std::prev(bus.busStops.begin());
        while(it != last)
        {
            ComputeDistance(it->, Coordinates to)
        }

        std::cout << "Bus " << bus.name << ": "
                  << bus.busStops.size() << " stops on route, "
                  << uniqueStops.size() << " unique stops, " << std::endl;
    }
};

