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

        bool operator==(const Stop& other) const
        {
            return (name == other.name) && (coord == other.coord);
        }
        bool operator<(const Stop& other) const
        {
            return std::tie(name, coord.lat, coord.lng) < std::tie(other.name, other.coord.lat, other.coord.lng);
        }
    };
    struct Bus
    {
        std::string name;
        std::vector<const Stop*> busStops;

        bool operator==(const Bus& other) const
        {
            return (name == other.name) && (busStops == other.busStops);
        }
    };

    void AddBus(Bus&& other)
    {
        if (buses.count(other)) {
            return;
        }
        buses.insert(std::move(other));
    }

    void AddBusStop(Stop&& stop)
    {
        busStops.push_back(std::move(stop));
    }

    [[nodiscard]] Bus GetBus(std::string name) const
    {
        auto it = std::find_if(buses.begin(),
                     buses.end(),
                     [&name](const Bus& bus){
            return bus.name == name;
        });
        if (!buses.count(*it)) {
            static Bus bus{};
            return bus;
        }
        return *it;
    }

    [[nodiscard]] Stop& GetStop(const std::string& name)
    {
        auto it = std::find_if(busStops.begin(),
                     busStops.end(),
                     [&name](const Stop& stop){
            return stop.name == name;
        });
        if (it == busStops.end()) {
            static Stop stop{};
            return stop;
        }
        return *it;
    }

    [[nodiscard]] auto& GetStops() const
    {
        return busStops;
    }

    [[nodiscard]] auto& GetBuses() const
    {
        return buses;
    }

    [[nodiscard]] Coordinates& GetStopCoords(const std::string& name)
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
    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = delete;
    ~TransportCatalogue() = default;

private:

    struct Hasher {
       size_t operator() (const Bus& bus) const {
            return std::hash<std::string>{}(bus.name)*37 + bus.busStops.size();
        }
    };

    std::deque<Stop> busStops{};
    std::unordered_set<Bus, Hasher> buses;
};
