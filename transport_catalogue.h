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
    using StopPointersVector = std::vector<std::string_view>;
    using BusPointersVector = std::unordered_set<std::string_view>;
    using Bus = std::string_view;

    struct Stop
    {
        bool operator==(const Stop& other) const
        {
            return (name == other.name);
        }

        std::string_view name;
        geo::Coordinates coord;
        std::unordered_map<std::string_view, double>;
    };

    void AddBus(Bus& bus)
    {
        busNames_.push_back(std::string{bus});
        bus = busNames_.back();
        buses_[busNames_.back()].first = bus;
    }

    void AddBusAndStops(Bus&& bus, std::vector<std::string>&& stops)
    {
        busNames_.push_back(std::string{bus});
        bus = busNames_.back();
        StopPointersVector v{};
        for (const auto& stop: stops)
        {
            auto it = std::find(stopNames_.begin(),
                                stopNames_.end(),
                                stop);
            if (it == stopNames_.end()) {
                continue;
            }
            v.push_back(*it);
        }
        buses_[busNames_.back()].first = bus;
        buses_[busNames_.back()].second = std::move(v);
    }

    void AddBusToStop(std::string_view name, std::string_view bus)
    {
        stops_[name].second.insert(bus);
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

    [[nodiscard]] const Bus& GetBus(std::string_view name) const
    {
        return buses_.at(name).first;
    }

    [[nodiscard]] std::vector<std::string_view>& GetBusStops(std::string_view name)
    {
        return buses_[name].second;
    }

    [[nodiscard]] const std::vector<std::string_view>& GetBusStops(std::string_view name) const
    {
        return buses_.at(name).second;
    }

    [[nodiscard]] Stop& GetStop(std::string_view name)
    {
        return stops_[name].first;
    }

    [[nodiscard]] const Stop& GetStop(std::string_view name) const
    {
        return stops_.at(name).first;
    }

    [[nodiscard]] auto& GetBuses()
    {
        return buses_;
    }

    [[nodiscard]] auto& GetBuses() const
    {
        return buses_;
    }

    [[nodiscard]] auto& GetStops()
    {
        return stops_;
    }

    [[nodiscard]] auto& GetStops() const
    {
        return stops_;
    }

    [[nodiscard]] geo::Coordinates& GetStopCoords(std::string_view name)
    {
        return stops_[name].first.coord;
    }

    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const
    {
        return stops_.at(name).first.coord;
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
    std::unordered_map<std::string_view, std::pair<Bus, StopPointersVector>> buses_{};
};
