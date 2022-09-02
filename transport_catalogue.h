#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <cassert>
#include "geo.h"

class TransportCatalogue
{
public:
    struct Stop;
    using StopPointersContainer = std::vector<std::string_view>;
    using BusPointersContainer = std::unordered_set<std::string_view>;
    using Bus = std::string_view;
    using RetParseDistancesBetweenElement = std::tuple<std::string, std::string, std::uint32_t>;
    using RetParseDistancesBetween = std::vector<RetParseDistancesBetweenElement>;
    using DistanceBetween = std::pair<const Stop*, const Stop*>;
    using RetParseBus = std::pair<std::string, std::vector<std::string>>;
    using RetParseStop = std::pair<std::string, geo::Coordinates>;

    struct Stop
    {
        bool operator==(const Stop& other) const
        {
            return (name == other.name);
        }

        std::string_view name;
        geo::Coordinates coord;
    };

    struct DistanceBetweenHasher
    {
        std::size_t operator()(const DistanceBetween& db) const
        {
            return std::hash<const void*>{}(db.first)*37 +
                    std::hash<const void*>{}(db.second);
        }
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
        StopPointersContainer v{};
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

    void AddStop(RetParseStop&& stop)
    {
        stopNames_.push_back(std::move(stop.first));
        stops_[stopNames_.back()].first = Stop{stopNames_.back(), std::move(stop.second)};
    }

    void AddDistances(const RetParseDistancesBetween& dbs)
    {
        for (const auto& db: dbs)
        {
            const std::string& nameA = std::get<0>(db);
            const std::string& nameB = std::get<1>(db);

            auto itA = std::find_if(stops_.begin(),
                                    stops_.end(),
                                    [&nameA](const auto& stop){
                return nameA == stop.first;
            });
            auto itB = std::find_if(stops_.begin(),
                                    stops_.end(),
                                    [&nameB](const auto& stop){
                return nameB == stop.first;
            });

            auto& stopA = itA->second.first;
            auto& stopB = itB->second.first;
            auto d = std::make_pair(&stopA, &stopB);
            distances_[d] = std::get<2>(db);
        }
    }

    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const
    {
        auto itA = std::find_if(stops_.begin(),
                                stops_.end(),
                                [&nameA](const auto& stop){
            return nameA == stop.first;
        });
        auto itB = std::find_if(stops_.begin(),
                                stops_.end(),
                                [&nameB](const auto& stop){
            return nameB == stop.first;
        });
        auto& stopA = itA->second.first;
        auto& stopB = itB->second.first;
        auto d1 = std::make_pair(&stopA, &stopB);
        auto d2 = std::make_pair(&stopB, &stopA);

        return distances_.count(d1) ? distances_.at(d1) :
                                      distances_.count(d2) ? distances_.at(d2) : 0UL;
    }

    [[nodiscard]] Bus& GetBus(std::string_view busName)
    {
        if (!buses_.count(busName)) {
            static Bus empty{};
            return empty;
        }
        return buses_[busName].first;
    }

    [[nodiscard]] const Bus& GetBus(std::string_view busName) const
    {
        if (!buses_.count(busName)) {
            static Bus empty{};
            return empty;
        }
        return buses_.at(busName).first;
    }

    [[nodiscard]] std::vector<std::string_view>& GetBusStops(std::string_view busName)
    {
        if (!buses_.count(busName)) {
            static std::vector<std::string_view> empty{};
            return empty;
        }
        return buses_[busName].second;
    }

    [[nodiscard]] const std::vector<std::string_view>& GetBusStops(std::string_view busName) const
    {
        if (!buses_.count(busName)) {
            static std::vector<std::string_view> empty{};
            return empty;
        }
        return buses_.at(busName).second;
    }

    [[nodiscard]] Stop& GetStop(std::string_view stopName)
    {
        return stops_[stopName].first;
    }

    [[nodiscard]] const Stop& GetStop(std::string_view stopName) const
    {
        return stops_.at(stopName).first;
    }

    [[nodiscard]] auto& GetBuses() noexcept
    {
        return buses_;
    }

    [[nodiscard]] auto& GetBuses() const noexcept
    {
        return buses_;
    }

    [[nodiscard]] auto& GetStops() noexcept
    {
        return stops_;
    }

    [[nodiscard]] auto& GetStops() const noexcept
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

    std::deque<std::string> stopNames_{};
    std::unordered_map<std::string_view, std::pair<Stop, BusPointersContainer>> stops_{};
    std::deque<std::string> busNames_{};
    std::unordered_map<std::string_view, std::pair<Bus, StopPointersContainer>> buses_{};
    std::unordered_map<DistanceBetween, std::uint32_t, DistanceBetweenHasher> distances_{};
};
