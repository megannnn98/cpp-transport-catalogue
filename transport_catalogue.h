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

    void AddStop(std::string_view name, const geo::Coordinates coord);
    [[nodiscard]] const Stop& GetStop(std::string_view name) const;
    void AddBusesToStop(std::string_view stopName, const std::vector<std::string>& busNames);
    void AddBusToStop(std::string_view stopName, std::string_view busName);
    void AddBus(std::string_view bus, const std::vector<std::string>& stopNames, bool isCircle);
    [[nodiscard]] const Bus& GetBus(std::string_view name) const;
    [[nodiscard]] const std::vector<const Stop*>& GetBusStops(std::string_view name) const;
    [[nodiscard]] bool IsBusCircle(std::string_view name) const;
    [[nodiscard]] std::unordered_map<std::string_view, std::pair<Bus*, StopPointersContainer>>& GetBuses() noexcept;
    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<Bus*, StopPointersContainer>>& GetBuses() const noexcept;
    [[nodiscard]] std::unordered_map<std::string_view, std::pair<Stop*, BusPointersContainer>>& GetStops() noexcept;
    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<Stop*, BusPointersContainer>>& GetStops() const noexcept;
    void AddDistances(const RetParseDistancesBetween& dbs);
    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const;
    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const;

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