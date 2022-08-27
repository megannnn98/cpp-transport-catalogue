#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class StatReader
{
    TransportCatalogue& tc_;
    InputReader& ir_;
public:
    StatReader(const StatReader&) = delete;
    StatReader& operator=(const StatReader&) = delete;
    StatReader(StatReader&&) = delete;
    StatReader& operator=(StatReader&&) = delete;
    ~StatReader() = default;
    StatReader(TransportCatalogue& tc, InputReader& ir) : tc_(tc), ir_(ir) {}

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;

    void PrintBus(std::string name)
    {
        auto bus = tc_.GetBus(name);
        if (bus.busStops.empty()) {
            std::cout << "Bus " << bus.name << ": " << "not found" << std::endl;
            return;
        }

        auto uniqCalc = [](const auto& stops) ->size_t {
            std::vector<Stop> uniqueStops(stops.size());

            for (std::size_t i{}; i < stops.size(); ++i) {
                uniqueStops[i] = *stops[i];
            }

            return std::distance(uniqueStops.begin(), std::unique(uniqueStops.begin(), uniqueStops.end()));
        };

        auto distanceCalc = [](const auto& stops) -> double {
            double ret{};
            auto it = stops.begin();
            auto last = std::prev(stops.end());
            while(it != last)
            {
                auto it1 = std::next(it);
                auto& stopCoord1 = (*it)->coord;
                auto& stopCoord2 = (*it1)->coord;

                ret += ComputeDistance(stopCoord1, stopCoord2);
                ++it;
            }
            return ret;
        };


        // Bus X: R stops on route, U unique stops, L route length
        std::cout << "Bus " << bus.name << ": "
                  << bus.busStops.size() << " stops on route, "
                  << uniqCalc(bus.busStops) << " unique stops, "
                  << std::setprecision(6) << distanceCalc(bus.busStops) << " route length"<< std::endl;
    }

    void ProcessRequest(std::istream& input)
    {
        std::string line{};
        std::vector<std::string> busDataLines{};

        std::getline(input, line);
        ir_.ltrim(line);
        ir_.rtrim(line);
        auto lineCnt = std::atoi(line.c_str());

        while (lineCnt--) {
            std::getline(input, line);
            ir_.ltrim(line);
            ir_.rtrim(line);
            if (line.length() <= 0) {
                continue;
            }
            if (line.find("Bus") == 0)
            {
                PrintBus(line.substr(4, line.size() - 4));
            }
        } // end while

    }


};

