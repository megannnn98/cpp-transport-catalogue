#include "request_handler.h"

namespace tc
{
RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::RenderSettings& settings)
    : db_{db}, renderer_{settings}
{
}

[[nodiscard]] const std::unordered_set<const domain::Bus*>& RequestHandler::GetBusesFromStop(std::string_view name) const
{
    return db_.GetBusesFromStop(name);
}

[[nodiscard]] const domain::Stop& RequestHandler::GetStop(std::string_view name) const
{
    return db_.GetStop(name);
}

[[nodiscard]] const domain::Bus& RequestHandler::GetBus(std::string_view name) const
{
    return db_.GetBus(name);
}

[[nodiscard]] const std::vector<const domain::Stop*>& RequestHandler::GetStopsFromBus(std::string_view name) const
{
    return db_.GetStopsFromBus(name);
}

[[nodiscard]] const geo::Coordinates& RequestHandler::GetStopCoords(std::string_view name) const
{
    return db_.GetStopCoords(name);
}

std::uint32_t RequestHandler::GetDistanceBetween(std::string_view nameA, std::string_view nameB) const
{
    return db_.GetDistanceBetween(nameA, nameB);
}

void RequestHandler::MakeDoc(svg::Document& doc) const
{
    renderer_.MakeDoc(doc, db_.GetBuses());
}
} // namespace
