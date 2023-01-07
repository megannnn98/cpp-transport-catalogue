#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <utility>
#include <string>
#include <string_view>

class RequestHandler {
public:
    RequestHandler(const tc::Catalogue& catalogue,
        const tc::Router& router, const renderer::MapRenderer& renderer);

    void JsonStatRequests(const json::Node& json_doc, std::ostream& output);

    svg::Document RenderMap() const;

private:
    const tc::Catalogue& db_;
    const tc::Router& router_;
    const renderer::MapRenderer& renderer_;

    json::Node FindStopRequestProcessing(const json::Dict& request_map);
    json::Node FindBusRequestProcessing(const json::Dict& request_map);
    json::Node BuildMapRequestProcessing(const json::Dict& request_map);
    json::Node BuildRouteRequestProcessing(const json::Dict& request_map);
};