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

    void AddStop(std::string_view name, geo::Coordinates coord)
    {
        stops_.push_back(Stop{std::string{name}, coord});
        stopnameToStop_.insert(std::make_pair(stops_.back().name, &stops_.back()));
    }

    [[nodiscard]] const Stop& GetStop(std::string_view name) const
    {
        if (!stopnameToStop_.count(name)){
            static Stop stop{};
            return stop;
        }
        return *stopnameToStop_.at(name);
    }

    void AddBus(std::string_view bus, const std::vector<std::string>& stopNames, bool isCircle)
    {
        buses_.push_back(Bus{std::string{bus}, isCircle});

        std::vector<const Stop*> stopPointers{};
        stopPointers.reserve(stopNames.size());
        for (const auto& stopName: stopNames)
        {
            auto& stop = GetStop(stopName);
            stopPointers.push_back(&stop);
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

    TransportCatalogue() = default;
    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = delete;
    ~TransportCatalogue() = default;

private:
    std::deque<Stop> stops_{};
    std::unordered_map<std::string_view, Stop*> stopnameToStop_{};
    std::deque<Bus> buses_{};
    std::unordered_map<std::string_view, std::pair<Bus*, std::vector<const Stop*>>> busnameToBus_{};
};
