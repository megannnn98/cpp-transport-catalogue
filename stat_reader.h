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
    StatReader& operator=(const StatReader&) = default;
    StatReader(StatReader&&) = default;
    StatReader& operator=(StatReader&&) = default;
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
        std::vector<const Stop*> uniqueStops{};
        std::copy(bus.busStops.begin(),
                  bus.busStops.end(), std::back_inserter(uniqueStops));

        auto lastUnique = std::unique(uniqueStops.begin(), uniqueStops.end(), [](const Stop* s1, const Stop* s2){
            return s1->name == s2->name;
        });
        uniqueStops.erase(lastUnique, uniqueStops.end());

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
                  << uniqueStops.size() << " unique stops, "
                  << std::setprecision(6) << distance << " route length"<< std::endl;
    }
};

