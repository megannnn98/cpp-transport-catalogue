#include <vector>
#include <string>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <cassert>
#include "geo.h"
#include "transport_catalogue.h"

namespace tc
{
    void TransportCatalogue::AddStop(std::string_view name, const geo::Coordinates coord)
    {
        stops_.push_back(Stop{std::string{name}, coord});
        stopnameToStop_[stops_.back().name] = std::make_pair(&stops_.back(), std::unordered_set<const Bus*>{});
    }

    [[nodiscard]] const TransportCatalogue::Stop& TransportCatalogue::GetStop(std::string_view name) const
    {
        if (!stopnameToStop_.count(name)){
            static Stop stop{};
            return stop;
        }
        return *stopnameToStop_.at(name).first;
    }

    void TransportCatalogue::AddBusesToStop(std::string_view stopName, const std::vector<std::string>& busNames)
    {
        std::unordered_set<const Bus*> busPointers{};
        for (const auto& busName: busNames)
        {
            auto& bus = GetBus(busName);
            busPointers.insert(&bus);
        }
        stopnameToStop_.at(stopName).second = std::move(busPointers);
    }

    void TransportCatalogue::AddBusToStop(std::string_view stopName, std::string_view busName)
    {
        auto& bus = GetBus(busName);
        stopnameToStop_.at(stopName).second.insert(&bus);
    }

    void TransportCatalogue::AddBus(std::string_view bus, const std::vector<std::string>& stopNames, bool isCircle)
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

    [[nodiscard]] const TransportCatalogue::Bus& TransportCatalogue::GetBus(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            static Bus bus{};
            return bus;
        }
        return *busnameToBus_.at(name).first;
    }

    [[nodiscard]] const std::vector<const TransportCatalogue::Stop*>& TransportCatalogue::GetBusStops(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            static std::vector<const Stop*> stops{};
            return stops;
        }
        return busnameToBus_.at(name).second;
    }

    [[nodiscard]] bool TransportCatalogue::IsBusCircle(std::string_view name) const
    {
        if (!busnameToBus_.count(name)){
            return false;
        }
        return busnameToBus_.at(name).first->isCircle;
    }

    [[nodiscard]] std::unordered_map<std::string_view, std::pair<TransportCatalogue::Bus*, TransportCatalogue::StopPointersContainer>>& TransportCatalogue::GetBuses() noexcept
    {
        return busnameToBus_;
    }

    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<TransportCatalogue::Bus*, TransportCatalogue::StopPointersContainer>>& TransportCatalogue::GetBuses() const noexcept
    {
        return busnameToBus_;
    }

    [[nodiscard]] std::unordered_map<std::string_view, std::pair<TransportCatalogue::Stop*, TransportCatalogue::BusPointersContainer>>& TransportCatalogue::GetStops() noexcept
    {
        return stopnameToStop_;
    }

    [[nodiscard]] const std::unordered_map<std::string_view, std::pair<TransportCatalogue::Stop*, TransportCatalogue::BusPointersContainer>>& TransportCatalogue::GetStops() const noexcept
    {
        return stopnameToStop_;
    }

    void TransportCatalogue::AddDistances(const RetParseDistancesBetween& dbs)
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

    std::uint32_t TransportCatalogue::GetDistanceBetween(std::string_view nameA, std::string_view nameB) const
    {
        auto& stopA = GetStop(nameA);
        auto& stopB = GetStop(nameB);
        auto d1 = std::make_pair(&stopA, &stopB);
        auto d2 = std::make_pair(&stopB, &stopA);

        return distances_.count(d1) ? distances_.at(d1) :
                                      distances_.count(d2) ? distances_.at(d2) : 0UL;
    }

    [[nodiscard]] const geo::Coordinates& TransportCatalogue::GetStopCoords(std::string_view name) const
    {
        return GetStop(name).coord;
    }
} // namespace tc