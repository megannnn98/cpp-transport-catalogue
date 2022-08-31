#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <list>
#include <algorithm>
#include "geo.h"

class TransportCatalogue
{
    using StopPointersVector = std::vector<std::string*>;
    using BusPointersVector = std::vector<std::string*>;
public:
    struct Bus;
    struct Stop
    {
        struct Hasher {
           size_t operator() (const Stop& stop) const {
                return std::hash<std::string_view>{}(stop.name)*37 + stop.coord.lat;
            }
        };

        bool operator==(const Stop& other) const
        {
            return (name == other.name);
        }

        std::string_view name;
        geo::Coordinates coord;
    };
    struct Bus
    {
        std::string_view name;

        struct Hasher {
           size_t operator() (const Bus& bus) const {
                return std::hash<std::string_view>{}(bus.name)*37;
            }
        };

        bool operator==(const Bus& other) const
        {
            return (name == other.name);
        }
    };

    void AddBus(Bus& bus)
    {
        busNames_.push_back(std::string{bus.name});
        bus.name = busNames_.back();
        buses_[busNames_.back()].first = bus;
    }

    void AddBusAndStops(Bus&& bus, std::vector<std::string>&& stops)
    {
        busNames_.push_back(std::string{bus.name});
        bus.name = busNames_.back();
        StopPointersVector v{};
        for (const auto& stop: stops)
        {
            auto it = std::find(stopNames_.begin(),
                                stopNames_.end(),
                                stop);
            if (it == stopNames_.end()) {
                continue;
            }
            v.push_back(&(*it));
        }
        buses_[busNames_.back()].first = bus;
        buses_[busNames_.back()].second = std::move(v);
    }

    void AddBusesToStop(std::string_view name, std::vector<std::string>&& buses)
    {
        auto& stop = GetStop(name);
        BusPointersVector v{};
        for (const auto& bus: buses)
        {
            auto it = std::find(busNames_.begin(),
                                busNames_.end(),
                                bus);
            if (it == busNames_.end()) {
                continue;
            }
            v.push_back(&(*it));
        }
    }

    void AddStop(Stop& stop)
    {
        stopNames_.push_back(std::string{stop.name});
        stop.name = stopNames_.back();
        stops_[stopNames_.back()].first = stop;
    }

    [[nodiscard]] Bus& GetBus(std::string_view name)
    {
        return buses_[name].first;
    }

    [[nodiscard]] std::vector<std::string*>& GetBusStops(std::string_view name)
    {
        return buses_[name].second;
    }

    [[nodiscard]] Stop& GetStop(std::string_view name)
    {
        return stops_[name].first;
    }

    [[nodiscard]] auto& GetBuses()
    {
        return buses_;
    }

    [[nodiscard]] auto& GetStops()
    {
        return stops_;
    }

    [[nodiscard]] auto& GetBuses() const
    {
        return buses_;
    }

    [[nodiscard]] geo::Coordinates& GetStopCoords(std::string_view name)
    {
        return stops_[name].first.coord;
    }


    TransportCatalogue() = default;
    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = delete;
    ~TransportCatalogue() = default;

private:

    std::list<std::string> stopNames_{};
    std::unordered_map<std::string_view, std::pair<Stop, BusPointersVector>> stops_{};
    std::list<std::string> busNames_{};
    std::unordered_map<std::string_view, std::pair<Bus, StopPointersVector>> buses_;
};
