#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <list>
#include <algorithm>
#include "geo.h"

class TransportCatalogue
{
public:
    struct Bus;
    struct Stop
    {
        struct Hasher {
           size_t operator() (const Stop& stop) const {
                return std::hash<std::string>{}(stop.name)*37 + stop.coord.lat;
            }
        };

        bool operator==(const Stop& other) const
        {
            return (name == other.name);
        }

        std::string name;
        geo::Coordinates coord;
    };
    struct Bus
    {
        std::string name;
        std::vector<const Stop*> stops;

        struct Hasher {
           size_t operator() (const Bus& bus) const {
                return std::hash<std::string>{}(bus.name)*37 + bus.stops.size();
            }
        };

        bool operator==(const Bus& other) const
        {
            return (name == other.name) && (stops == other.stops);
        }
    };

    void AddBus(Bus&& other)
    {
        if (buses_.count(other)) {
            return;
        }
        buses_.insert(std::move(other));
    }

    void AddBusStop(Stop&& stop)
    {
        stops_.push_back(std::move(stop));
    }

    [[nodiscard]] Bus GetBus(std::string_view name) const
    {
        auto it = std::find_if(buses_.begin(),
                     buses_.end(),
                     [&name](const Bus& bus){
            return bus.name == name;
        });
        if (it == buses_.end()) {
            static Bus bus{};
            return bus;
        }
        return *it;
    }

    [[nodiscard]] Stop& GetStop(std::string_view name)
    {
        auto it = std::find_if(stops_.begin(),
                     stops_.end(),
                     [&name](const Stop& stop){
            return stop.name == name;
        });
        if (it == stops_.end()) {
            static Stop stop{};
            return stop;
        }
        return *it;
    }

    [[nodiscard]] auto& GetStops()
    {
        return stops_;
    }

    [[nodiscard]] auto& GetBuses()
    {
        return buses_;
    }

    [[nodiscard]] auto& GetStops() const
    {
        return stops_;
    }

    [[nodiscard]] auto& GetBuses() const
    {
        return buses_;
    }

    [[nodiscard]] geo::Coordinates& GetStopCoords(std::string_view name)
    {
        auto it = std::find_if(stops_.begin(),
                            stops_.end(),
                               [&name](const Stop& stop){
            return name == stop.name;
        });
        if (it == stops_.end()) {
            static geo::Coordinates coord{};
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
    std::list<Stop> stops_{};
    std::unordered_set<Bus, Bus::Hasher> buses_;
};
