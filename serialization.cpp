#include "serialization.h"

using namespace std;

void Serialize(const transport::Catalogue& tcat, std::ostream& output) {
    serialize::TransportCatalogue database;
    for (const auto& [name, s] : tcat.GetSortedAllStops()) {
        *database.add_stop() = Serialize(s);
    }
    for (const auto& [name, b] : tcat.GetSortedAllBuses()) {
        *database.add_bus() = Serialize(b);
    }
    database.SerializeToOstream(&output);
}

serialize::Stop Serialize(const transport::Stop* stop) {
    serialize::Stop result;
    result.set_name(stop->name);
    result.add_coordinate(stop->coordinates.lat);
    result.add_coordinate(stop->coordinates.lng);
    for (const auto& [n, d] : stop->stop_distances) {
        result.add_near_stop(static_cast<string>(n));
        result.add_distance(d);
    }
    return result;
}

serialize::Bus Serialize(const transport::Bus* bus) {
    serialize::Bus result;
    result.set_name(bus->name);
    for (const auto& s : bus->stops) {
        result.add_stop(s->name);
    }
    result.set_is_circle(bus->is_circle);
    if (bus->final_stop)
        result.set_final_stop(bus->final_stop->name);
    return result;
}

void SetStopsDistances(transport::Catalogue& tcat, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.stop_size(); ++i) {
        const serialize::Stop& stop_i = database.stop(i);
        transport::Stop* from = tcat.FindStop(stop_i.name());
        for (size_t j = 0; j < stop_i.near_stop_size(); ++j) {
            tcat.SetDistance(from, tcat.FindStop(stop_i.near_stop(j)), stop_i.distance(j));
        }
    }
}

void AddStopFromDB(transport::Catalogue& tcat, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.stop_size(); ++i) {
        const serialize::Stop& stop_i = database.stop(i);
        tcat.AddStop(stop_i.name(), { stop_i.coordinate(0), stop_i.coordinate(1) });
    }
    SetStopsDistances(tcat, database);
}

void AddBusFromDB(transport::Catalogue& tcat, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.bus_size(); ++i) {
        const serialize::Bus& bus_i = database.bus(i);
        std::vector<transport::Stop*> stops(bus_i.stop_size());
        for (size_t j = 0; j < stops.size(); ++j) {
            stops[j] = tcat.FindStop(bus_i.stop(j));
        }
        tcat.AddBus(bus_i.name(), stops, bus_i.is_circle());
        if (!bus_i.final_stop().empty()) {
            transport::Bus* bus = tcat.FindBus(bus_i.name());
            bus->final_stop = tcat.FindStop(bus_i.final_stop());
        }
    }
}

std::tuple<transport::Catalogue, renderer::MapRenderer, transport::Router>
    Deserialize(std::istream& input) {
    serialize::TransportCatalogue database;
    database.ParseFromIstream(&input);
    transport::Catalogue tcat;
    renderer::MapRenderer renderer{};
    transport::Router router{};
    AddStopFromDB(tcat, database);
    AddBusFromDB(tcat, database);
    return { std::move(tcat), std::move(renderer), std::move(router)};
}
