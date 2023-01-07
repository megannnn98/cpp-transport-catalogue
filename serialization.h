#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

void Serialize(const tc::Catalogue& tcat,
    const renderer::MapRenderer& renderer,
    const tc::Router& router,
    std::ostream& output);

serialize::Stop Serialize(const tc::Stop* stop);

serialize::Bus Serialize(const tc::Bus* bus);

serialize::RenderSettings GetRenderSettingSerialize(const json::Node& render_settings);

serialize::RouterSettings GetRouterSettingSerialize(const json::Node& router_settings);

serialize::Router Serialize(const tc::Router& router);

std::tuple<tc::Catalogue, renderer::MapRenderer, tc::Router,
    graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);