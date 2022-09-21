#pragma once

#include "transport_catalogue.h"
#include <ostream>
#include <iostream>
#include <iomanip>
#include <stack>
#include <optional>
#include "domain.h"
#include "map_renderer.h"
#include "json.h"
#include <set>

namespace tc {

using namespace std::string_literals;
class RequestHandler
{
    const TransportCatalogue& db_;
    const renderer::MapRenderer renderer_;
public:
    explicit RequestHandler(const TransportCatalogue& db, const renderer::RenderSettings& settings)
        : db_{db}, renderer_{settings}
    {
    }

    [[nodiscard]] const auto& GetBusesFromStop(std::string_view name) const
    {
        return db_.GetBusesFromStop(name);
    }

    [[nodiscard]] const domain::Stop& GetStop(std::string_view name) const
    {
        return db_.GetStop(name);
    }

    [[nodiscard]] const domain::Bus& GetBus(std::string_view name) const
    {
        return db_.GetBus(name);
    }

    [[nodiscard]] const std::vector<const domain::Stop*>& GetStopsFromBus(std::string_view name) const
    {
        return db_.GetStopsFromBus(name);
    }

    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const
    {
        return db_.GetStopCoords(name);
    }

    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const
    {
        return db_.GetDistanceBetween(nameA, nameB);
    }

    void MakeDoc(svg::Document& doc) const
    {
        renderer_.MakeDoc(doc, db_.GetBuses());
    }

};

} // namespace tc
