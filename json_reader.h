#pragma once

#include <istream>
#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace json
{
class JsonReader
{
    std::istream& is_;
    json::Document doc_;
    void ExtractStopsWithCoords(tc::TransportCatalogue& tc, const json::Array& arr);
    void ExtractBusWithStops(tc::TransportCatalogue& tc, const json::Array& arr);
    void ExtractDistancesBetweenStops(tc::TransportCatalogue& tc, const json::Array& arr);
    void AddBusesToStops(tc::TransportCatalogue& tc);
public:
    explicit JsonReader(std::istream& is);

    tc::TransportCatalogue MakeCatalogue();
    void Print(const tc::RequestHandler& handler, std::ostream& os) const;
    renderer::RenderSettings MakeRenderSettings();
};

} // namespace
