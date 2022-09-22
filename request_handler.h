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
    RequestHandler(const TransportCatalogue& db, const renderer::RenderSettings& settings);
    [[nodiscard]] const std::unordered_set<const domain::Bus*>& GetBusesFromStop(std::string_view name) const;
    [[nodiscard]] const domain::Stop& GetStop(std::string_view name) const;
    [[nodiscard]] const domain::Bus& GetBus(std::string_view name) const;
    [[nodiscard]] const std::vector<const domain::Stop*>& GetStopsFromBus(std::string_view name) const;
    [[nodiscard]] const geo::Coordinates& GetStopCoords(std::string_view name) const;
    std::uint32_t GetDistanceBetween(std::string_view nameA, std::string_view nameB) const;
    void MakeDoc(svg::Document& doc) const;

};

} // namespace tc
