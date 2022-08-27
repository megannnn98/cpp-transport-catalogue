#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include "geo.h"

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
        Coordinates coord;
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

    std::deque<Stop>& GetStops()
    {
        return busStops;
    }

    std::deque<Bus>& GetBuses()
    {
        return buses;
    }

    Coordinates& GetStopCoords(const std::string& name)
    {
        auto it = std::find_if(busStops.begin(),
                            busStops.end(),
                               [&name](const Stop& stop){
            return name == stop.name;
        });
        if (it == busStops.end()) {
            static Coordinates coord{};
            return coord;
        }
        return it->coord;
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
