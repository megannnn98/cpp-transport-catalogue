#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class StatReader
{
    TransportCatalogue& tc_;
public:
    StatReader(const StatReader&) = default;
    StatReader& operator=(const StatReader&) = delete;
    StatReader(StatReader&&) = delete;
    StatReader& operator=(StatReader&&) = delete;
    ~StatReader() = default;
    StatReader(TransportCatalogue& tc) : tc_(tc) {}

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;

    void PrintBus(std::string name)
    {
        auto bus = tc_.GetBus(name);
        if (bus.busStops.empty())
        {
            std::cout << "Bus " << bus.name << ": " << "not found" << std::endl;
            return;
        }
        std::vector<Stop> uniqueStops(bus.busStops.size());

        for (std::size_t i{}; i < bus.busStops.size(); ++i) {
            uniqueStops[i] = *bus.busStops[i];
        }

        auto uniqNum = std::distance(uniqueStops.begin(), std::unique(uniqueStops.begin(), uniqueStops.end()));

        double distance{};
        auto it = bus.busStops.begin();
        auto last = std::prev(bus.busStops.end());
        while(it != last)
        {
            auto it1 = std::next(it);
            auto& stopCoord1 = (*it)->coord;
            auto& stopCoord2 = (*it1)->coord;

            distance += ComputeDistance(stopCoord1, stopCoord2);
            ++it;
        }

        // Bus X: R stops on route, U unique stops, L route length
        std::cout << "Bus " << bus.name << ": "
                  << bus.busStops.size() << " stops on route, "
                  << uniqNum << " unique stops, "
                  << std::setprecision(6) << distance << " route length"<< std::endl;
    }
};

