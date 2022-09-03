#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <cassert>
#include "geo.h"

namespace tc 
{
class TransportCatalogue
{
public:
    struct Stop;
    struct Bus;
    using StopPointersContainer = std::vector<const Stop*>;
    using BusPointersContainer = std::unordered_set<const Bus*>;
    using RetParseDistancesBetweenElement = std::tuple<std::string, std::string, std::uint32_t>;
    using RetParseDistancesBetween = std::vector<RetParseDistancesBetweenElement>;
    using DistanceBetween = std::pair<const Stop*, const Stop*>;
    using RetParseBus = std::tuple<std::string, std::vector<std::string>, bool>;
    using RetParseStop = std::pair<std::string, geo::Coordinates>;

    struct Stop
    {
        bool operator==(const Stop& other) const
        {
            return (name == other.name);
        }

        std::string name;
        geo::Coordinates coord;
    };

    struct Bus
    {
        bool operator==(const Bus& other) const
        {
            return (name == other.name);
        }

        std::string name;
        bool isCircle{};
    };

    struct DistanceBetweenHasher
    {
        std::size_t operator()(const DistanceBetween& db) const
        {
            return std::hash<const void*>{}(db.first)*37 +
                    std::hash<const void*>{}(db.second);
        }
    };

    void AddStop(std::string_view name, const geo::Coordinates coord)
    {
        stops_.push_back(Stop{std::string{name}, coord});
        stopnameToStop_[stops_.back().name] = std::make_pair(&stops_.back(), std::unordered_set<const Bus*>{});
    }

    [[nodiscard]] const Stop& GetStop(std::string_view name) const
    {
        if (!stopnameToStop_.count(name)){
            static Stop stop{};
            return stop;
        }
        return *stopnameToStop_.at(name).first;
    }

    void AddBusesToStop(std::string_view stopName, const std::vector<std::string>& busNames)
    {
        std::unordered_set<const Bus*> busPointers{};
        for (const auto& busName: busNames)
        {
            auto& bus = GetBus(busName);
            busPointers.insert(&bus);
        }
        stopnameToStop_.at(stopName).second = std::move(busPointers);
    }

    void AddBusToStop(std::string_view stopName, std::string_view busName)
    {
        auto& bus = GetBus(busName);
        stopnameToStop_.at(stopName).second.insert(&bus);
    }

    void AddBus(std::string_view bus, const std::vector<std::string>& stopNames, bool isCircle)
    {
        buses_.push_back(Bus{std::string{bus}, isCircle});

        std::vector<const Stop*> stopPointers{};
        stopPointers.reserve(stopNames.size());
        for (const auto& stopName: stopNames)
        {
            auto& stopRef = GetStop(stopName);
            stopPointers.push_back(&stopRef);
        }
        busnameToBus_[buses_.back().name] = std::make_pair(&buses_.back(), std::move(stopPointers));
    }

    [[nodiscard]] const Bus& GetBus(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            static Bus bus{};
            return bus;
        }
        return *busnameToBus_.at(name).first;
    }

    [[nodiscard]] const std::vector<const Stop*>& GetBusStops(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            static std::vector<const Stop*> stops{};
            return stops;
        }
        return busnameToBus_.at(name).second;
    }

    [[nodiscard]] bool IsBusCircle(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            return false;
        }
        return busnameToBus_.at(name).first->isCircle;
    }

    [[nodiscard]] auto& GetBuses() noexcept
    {
        return busnameToBus_;
    }

    [[nodiscard]] auto& GetBuses() const noexcept
    {
        return busnameToBus_;
    }

    [[nodiscard]] auto& GetStops() noexcept
    {
        return stopnameToStop_;
    }

    [[nodiscard]] auto& GetStops() const noexcept
    {
        return stopnameToStop_;
    }

    void AddDistances(const RetParseDistancesBetween& dbs)
    {
        for (const auto& db: dbs)
        {
            const std::string& nameA = std::get<0>(db);
            const std::string& nameB = std::get<1>(db);
            auto& stopA = GetStop(nameA);
            auto& stopB = GetStop(nameB);
            auto d = std::make_pair(&stopA, &stopB);
            distances_[d] = std::get<2>(db);
        }
    }

    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const
    {
        auto& stopA = GetStop(nameA);
        auto& stopB = GetStop(nameB);
        auto d1 = std::make_pair(&stopA, &stopB);
        auto d2 = std::make_pair(&stopB, &stopA);

        return distances_.count(d1) ? distances_.at(d1) :
                                      distances_.count(d2) ? distances_.at(d2) : 0UL;
    }

    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const
    {
        return GetStop(name).coord;
    }

    TransportCatalogue() = default;
    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = delete;
    ~TransportCatalogue() = default;

private:
    std::deque<Stop> stops_{};
    std::unordered_map<std::string_view, std::pair<Stop*, BusPointersContainer>> stopnameToStop_{};
    std::deque<Bus> buses_{};
    std::unordered_map<std::string_view, std::pair<Bus*, StopPointersContainer>> busnameToBus_{};
    std::unordered_map<DistanceBetween, std::uint32_t, DistanceBetweenHasher> distances_{};
};
} // namespace tc 