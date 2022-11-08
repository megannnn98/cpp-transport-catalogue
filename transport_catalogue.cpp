#include <utility>
#include <unordered_set>
#include "transport_catalogue.h"

void tc::TransportCatalogue::AddStop(const std::string& name, double latitude, double longitude, int id) {
    geo::Coordinates coord;
    coord.lat = latitude;
    coord.lng = longitude;
    domain::Stop new_stop;
    new_stop.name = name;
    new_stop.coordinates = coord;
    new_stop.id_ = id;
    stops_.push_back(std::move(new_stop));
    stopname_to_stop_[stops_.back().name] = &stops_.back();
    stopname_to_busname_[&stops_.back()];
}

void tc::TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stops, bool rounded) {
    domain::Bus new_bus;
    new_bus.name = name;
    for (const std::string& stop_name : stops) {
        new_bus.route.push_back(stopname_to_stop_.at(stop_name));
    }
    new_bus.is_rounded = rounded;
    buses_.push_back(std::move(new_bus));
    busname_to_bus_[buses_.back().name] = &buses_.back();
    for (const auto& stop : buses_.back().route) {
        stopname_to_busname_[stop].insert(buses_.back().name);
    }
    auto result = ComputeDistanceBetweenStops();
    buses_.back().distance_real = result.first;
    buses_.back().distance_ideal = result.second;
}

std::pair <double, double> tc::TransportCatalogue::ComputeDistanceBetweenStops() {
    double distance_real = 0.0;
    double distance_ideal = 0.0;
    if (buses_.back().is_rounded == false) {
        for (size_t i = 0; i < buses_.back().route.size() - 1; ++i) {
            if (stops_to_distance.count({ buses_.back().route[i], buses_.back().route[i + 1] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i], buses_.back().route[i + 1] });
            }
            else if (stops_to_distance.count({ buses_.back().route[i + 1], buses_.back().route[i] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i + 1], buses_.back().route[i] });
            }
            else {
                distance_real += std::abs(ComputeDistance(buses_.back().route[i]->coordinates, buses_.back().route[i + 1]->coordinates));
            }
            distance_ideal += std::abs(ComputeDistance(buses_.back().route[i]->coordinates, buses_.back().route[i + 1]->coordinates));
        }
        for (size_t i = buses_.back().route.size() - 1; i >= 1; --i) {
            if (stops_to_distance.count({ buses_.back().route[i], buses_.back().route[i - 1] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i], buses_.back().route[i - 1] });
            }
            else if (stops_to_distance.count({ buses_.back().route[i - 1], buses_.back().route[i] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i - 1], buses_.back().route[i] });
            }
            else {
                distance_real += std::abs(ComputeDistance(buses_.back().route[i]->coordinates, buses_.back().route[i - 1]->coordinates));
            }
        }
        distance_ideal = distance_ideal * 2;
    }
    else {
        for (size_t i = 0; i < buses_.back().route.size() - 1; ++i) {
            if (stops_to_distance.count({ buses_.back().route[i], buses_.back().route[i + 1] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i], buses_.back().route[i + 1] });
            }
            else if (stops_to_distance.count({ buses_.back().route[i + 1], buses_.back().route[i] }) != 0) {
                distance_real += stops_to_distance.at({ buses_.back().route[i + 1], buses_.back().route[i] });
            }
            else {
                distance_real += std::abs(ComputeDistance(buses_.back().route[i]->coordinates, buses_.back().route[i + 1]->coordinates));
            }
            distance_ideal += std::abs(ComputeDistance(buses_.back().route[i]->coordinates, buses_.back().route[i + 1]->coordinates));
        }
    }
    return { distance_real, distance_ideal };
}

const std::optional<std::set<std::string_view>> tc::TransportCatalogue::BusesOnStop(const std::string_view& stop_name) const {
    if (stopname_to_stop_.count(stop_name) == 0) {
        return {};
    }
    std::set<std::string_view> result;
    if (stopname_to_busname_.at(stopname_to_stop_.at(stop_name)).size() == 0) {
        return result;
    }
    for (const auto& bus : stopname_to_busname_.at(stopname_to_stop_.at(stop_name))) {
        result.insert(bus);
    }
    return result;
}

const std::set<std::string_view> tc::TransportCatalogue::GetAllBusesNames() const {
    std::set<std::string_view> result;
    for (const auto [key, _] : busname_to_bus_) {
        result.insert(key);
    }
    return result;
}

const std::vector<std::string_view> tc::TransportCatalogue::GetBusRoute(const std::string_view& bus_name) const {
    std::vector<std::string_view> result;
    if (busname_to_bus_.count(bus_name) != 0) {
        for (const auto& stop : busname_to_bus_.at(bus_name)->route) {
            result.push_back(stop->name);
        }
    }
    return result;
}

const geo::Coordinates tc::TransportCatalogue::GetStopCoordinates(const std::string_view& stop_name) const {
    geo::Coordinates result;
    if (stopname_to_stop_.at(stop_name) != 0) {
        result = stopname_to_stop_.at(stop_name)->coordinates;
    }
    return result;
}

const std::vector<geo::Coordinates> tc::TransportCatalogue::GetAllStopCoordinates() const {
    std::vector<geo::Coordinates> result;
    for (const auto& [name, stop] : stopname_to_stop_) {
        if (stopname_to_busname_.at(stop).size() != 0) {
            result.push_back(stopname_to_stop_.at(name)->coordinates);
        }
    }
    return result;
}

const std::optional<domain::Statistics> tc::TransportCatalogue::GetBusInfo(const std::string_view& bus) const {
    if (busname_to_bus_.count(bus) == 0) {
        return {};
    }
    domain::Statistics result;
    result.found = true;
    if (busname_to_bus_.at(bus)->is_rounded == false) {
        result.is_rounded = false;
        result.stops_count = busname_to_bus_.at(bus)->route.size();
        result.stops_count = (result.stops_count * 2) - 1;
    }
    else {
        result.is_rounded = true;
        result.stops_count = busname_to_bus_.at(bus)->route.size();
    }
    int unique_count = 0;
    std::unordered_set<std::string_view> unique_stops_names;
    for (const auto& stop : busname_to_bus_.at(bus)->route) {
        if (unique_stops_names.count(stop->name) == 0) {
            unique_stops_names.insert(stop->name);
            ++unique_count;
        }
    }
    result.unique_stops_count = unique_count;
    result.distance = busname_to_bus_.at(bus)->distance_real;
    result.curvature = busname_to_bus_.at(bus)->distance_real / busname_to_bus_.at(bus)->distance_ideal;
    return result;
}

void tc::TransportCatalogue::AddStopDistances(const std::string& stop_name, const std::vector<std::pair<std::string, int>>& stops_and_distances) {
    for (const auto& info : stops_and_distances) {
        stops_to_distance[{stopname_to_stop_[stop_name], stopname_to_stop_[info.first]}] = info.second;
    }
}

const domain::Stop& tc::TransportCatalogue::GetStopByName(const std::string_view& stop_name) const {
    return *(stopname_to_stop_.at(stop_name));
}

size_t tc::TransportCatalogue::GetAllStopsCount() const {
    return stopname_to_stop_.size();
}

int tc::TransportCatalogue::GetStopToStopDistance(const std::string_view& from, const std::string_view& to) const {
    if (stops_to_distance.count({ stopname_to_stop_.at(from), stopname_to_stop_.at(to) }) != 0) {
        return stops_to_distance.at({ stopname_to_stop_.at(from), stopname_to_stop_.at(to) });
    }
    if (stops_to_distance.count({ stopname_to_stop_.at(to), stopname_to_stop_.at(from) }) != 0) {
        return stops_to_distance.at({ stopname_to_stop_.at(to), stopname_to_stop_.at(from) });
    }
    return std::abs(ComputeDistance(stopname_to_stop_.at(from)->coordinates, stopname_to_stop_.at(to)->coordinates));
}

bool tc::TransportCatalogue::IsRoundBus(const std::string_view& bus_name) const {
    return busname_to_bus_.at(bus_name)->is_rounded;
}

bool tc::TransportCatalogue::CheckStopValidity(const std::string_view& stop_name) const {
    return stopname_to_stop_.count(stop_name);
}

const std::unordered_map<std::string_view, const domain::Bus*> tc::TransportCatalogue::GetAllBuses() const {
    return busname_to_bus_;
}