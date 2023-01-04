#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

void Serialize(const transport::Catalogue& tcat, std::ostream& output);

serialize::Stop Serialize(const transport::Stop* stop);

serialize::Bus Serialize(const transport::Bus* bus);

std::tuple<transport::Catalogue, renderer::MapRenderer, transport::Router>
    Deserialize(std::istream& input);
