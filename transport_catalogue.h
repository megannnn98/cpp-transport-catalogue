#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>

//добавление маршрута в базу,
//добавление остановки в базу,
//поиск маршрута по имени,
//поиск остановки по имени,
//получение информации о маршруте.

class TransportCatalogue
{
public:
    struct Stop
    {
        std::string name;
        float latitude;
        float longitude;
    };
    struct Bus
    {
        std::string name;
        std::vector<Stop> busStops;
    };

    void AddBus(Bus&& bus)
    {

    }

    void AddBusStop(Stop&& stop)
    {

    }

    Bus GetBus(std::string name) const
    {
        return {};
    }

    void PrintBus(int bus)
    {
        // Bus 256: 6 stops on bus, 5 unique stops, 4371.02 bus length
    }

    TransportCatalogue() = default;
    TransportCatalogue(const TransportCatalogue&) = default;
    TransportCatalogue& operator=(const TransportCatalogue&) = default;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = default;
    ~TransportCatalogue() = default;

private:
    std::deque<Stop> busStops{};
    std::deque<Bus> buses{};
    std::unordered_map<const Bus*, const Stop*> numToBus{};
};
