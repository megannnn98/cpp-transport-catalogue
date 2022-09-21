#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <cassert>
#include "geo.h"
#include "domain.h"

namespace tc
{
class TransportCatalogue
{
public:
    using StopPointersContainer = std::vector<const domain::Stop*>;
    using BusPointersContainer = std::unordered_set<const domain::Bus*>;
    using RetParseDistancesBetweenElement = std::tuple<std::string, std::string, std::uint32_t>;
    using RetParseDistancesBetween = std::vector<RetParseDistancesBetweenElement>;
    using DistanceBetween = std::pair<const domain::Stop*, const domain::Stop*>;
    using RetParseBus = std::tuple<std::string, std::vector<std::string>, bool>;
    using RetParseStop = std::pair<std::string, geo::Coordinates>;

    struct DistanceBetweenHasher
    {
        std::size_t operator()(const DistanceBetween& db) const
        {
            return std::hash<const void*>{}(db.first)*37 +
                    std::hash<const void*>{}(db.second);
        }
    };

    void AddStop(std::string_view name, const geo::Coordinates coord);
    [[nodiscard]] const domain::Stop& GetStop(std::string_view name) const;
    void AddBusesToStop(std::string_view stopName, const std::vector<std::string>& busNames);
    void AddBusToStop(std::string_view stopName, std::string_view busName);
    void AddBus(std::string_view bus, const std::vector<std::string>& stopNames, bool isCircle);
    [[nodiscard]] const domain::Bus& GetBus(std::string_view name) const;
    [[nodiscard]] std::unordered_map<std::string_view, std::pair<domain::Bus*, StopPointersContainer>>& GetBuses() noexcept;
    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<domain::Bus*, StopPointersContainer>>& GetBuses() const noexcept;
    [[nodiscard]] std::unordered_map<std::string_view, std::pair<domain::Stop*, BusPointersContainer>>& GetStops() noexcept;
    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<domain::Stop*, BusPointersContainer>>& GetStops() const noexcept;
    void AddDistances(const RetParseDistancesBetween& dbs);
    void AddDistances(std::string_view nameA, std::string_view nameB, std::uint32_t value);
    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const;
    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const;
    [[nodiscard]] const BusPointersContainer& GetBusesFromStop(std::string_view name) const;
    [[nodiscard]] const std::vector<const domain::Stop*>& GetStopsFromBus(std::string_view name) const;

    TransportCatalogue() = default;
    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;
    TransportCatalogue& operator=(TransportCatalogue&&) = delete;
    ~TransportCatalogue() = default;

private:
    std::deque<domain::Stop> stops_{};
    std::unordered_map<std::string_view, std::pair<domain::Stop*, BusPointersContainer>> stopnameToStop_{};
    std::deque<domain::Bus> buses_{};
    std::unordered_map<std::string_view, std::pair<domain::Bus*, StopPointersContainer>> busnameToBus_{};
    std::unordered_map<DistanceBetween, std::uint32_t, DistanceBetweenHasher> distances_{};
};
} // namespace tc
