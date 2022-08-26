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
        int name;
        std::vector<std::string> busStops;
    };

    void AddBus(Bus&& bus)
    {
        int name = bus.name;
        auto it = std::find_if(buses.begin(),
                            buses.end(),
                            [name](const Bus& bus ){
                                return bus.name == name;
                            });
        if ((it != buses.end()) && (bus.busStops.empty())) {
            return;
        }

        buses.push_back(std::move(bus));
    }

    void AddBusStop(Stop&& stop)
    {
        busStops.push_back(std::move(stop));
    }

    Bus GetBus(std::string name) const
    {
        auto it = std::find_if(buses.begin(),
                     buses.end(),
                     [&name](const Bus& bus){
            return bus.name == std::atoi(name.c_str());
        });
        return *it;
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
