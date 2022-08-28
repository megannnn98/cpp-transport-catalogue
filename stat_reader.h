#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <unordered_set>
#include <set>
#include <numeric>

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
            std::set<Stop> uniqueStops;

            for (std::size_t i{}; i < stops.size(); ++i) {
                uniqueStops.insert(*stops[i]);
            }
            return uniqueStops.size();
        };

        double distanceCalc = std::transform_reduce(bus.busStops.begin(),
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

