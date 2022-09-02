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

    [[nodiscard]] Stop& GetStop(std::string_view name)
    {
        auto it = std::find(stops_.begin(),
                            stops_.end(),
                            [&name](const Stop& s){
            return s.name == name;
        });

        if (it == stops_.end()){
            static Stop stop{};
            return stop;
        }

        return *it;
    }

    [[nodiscard]] const Stop& GetStop(std::string_view name) const
    {
        auto it = std::find(stops_.cbegin(),
                            stops_.cend(),
                            [&name](const Stop& s){
            return s.name == name;
        });

        if (it == stops_.end()){
            static const Stop stop{};
            return stop;
        }

        return *it;
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
};
