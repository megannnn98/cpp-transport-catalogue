#include "map_renderer.h"

void renderer::MapRenderer::MakeDoc(svg::Document& doc, const std::unordered_map<std::string_view, std::pair<domain::Bus*, std::vector<const domain::Stop*>>>& allBuses) const
{
    struct StopCmp {
        bool operator() (const domain::Stop* a, const domain::Stop* b) const {
            return std::lexicographical_compare(a->name.begin(), a->name.end(),
                                                b->name.begin(), b->name.end());
        }
    };
    struct BusCmp {
        bool operator() (const domain::Bus* a, const domain::Bus* b) const {
            return std::lexicographical_compare(a->name.begin(), a->name.end(),
                                                b->name.begin(), b->name.end());
        }
    };

    std::vector<geo::Coordinates> allCoords{};
    std::set<const domain::Stop*, StopCmp> allStops;
    for (auto& bus: allBuses)
    {
        for (const domain::Stop* stop: bus.second.second)
        {
            allCoords.push_back(stop->coord);
            allStops.insert(stop);
        }
    }

    renderer::SphereProjector sp(allCoords.begin(), allCoords.end(), settings_.width, settings_.height, settings_.padding);

    std::size_t cnt{};
    std::vector<Route> routesForDrawing{};
    std::vector<RouteText> busTextNames{};

    std::set<const domain::Bus*, BusCmp> sortedBuses{};
    for (const auto& bus: allBuses)
    {
        sortedBuses.insert(bus.second.first);
    }

    for (const auto* bus: sortedBuses)
    {
        if (allBuses.at(bus->name).second.empty()) {
            continue;
        }

        std::vector<svg::Point> points{};
        std::size_t stopNum{allBuses.at(bus->name).second.size()};
        for (std::size_t i{}; i < stopNum; ++i)
        {
            // bus->isCircle == true - конечной считается первая остановка маршрута.
            // bus->isCircle == false - в некольцевом — первая и последняя.

            svg::Point stopPos{sp(allBuses.at(bus->name).second.at(i)->coord)};
            if (i == 0)
            {
                    busTextNames.emplace_back(settings_.underlayer_color,
                                               settings_.underlayer_color,
                                               settings_.underlayer_width,
                                               stopPos,
                                               settings_.bus_label_offset,
                                               settings_.bus_label_font_size,
                                               "Verdana"s,
                                               "bold"s,
                                               bus->name);
                    busTextNames.emplace_back(settings_.color_palette.at(cnt),
                                               stopPos,
                                               settings_.bus_label_offset,
                                               settings_.bus_label_font_size,
                                               "Verdana"s,
                                               "bold"s,
                                               bus->name);
            }
            else if (!bus->isCircle &&
                     (i == ((stopNum-1)/2)) &&
                     (stopPos != sp(allBuses.at(bus->name).second.at(0)->coord)))
            {
                busTextNames.emplace_back(settings_.underlayer_color,
                                           settings_.underlayer_color,
                                           settings_.underlayer_width,
                                           stopPos,
                                           settings_.bus_label_offset,
                                           settings_.bus_label_font_size,
                                           "Verdana"s,
                                           "bold"s,
                                           bus->name);
                busTextNames.emplace_back(settings_.color_palette.at(cnt),
                                           stopPos,
                                           settings_.bus_label_offset,
                                           settings_.bus_label_font_size,
                                           "Verdana"s,
                                           "bold"s,
                                           bus->name);
            }

            points.emplace_back(stopPos);
        }


        Route route(std::move(points), settings_.line_width, settings_.color_palette.at(cnt));
        routesForDrawing.push_back(std::move(route));

        cnt++;
        cnt %= settings_.color_palette.size();
    }

    std::vector<StopCircle> stopCircles{};
    for (auto stop: allStops)
    {
        stopCircles.emplace_back(sp(stop->coord), settings_.stop_radius, "white"s);
    }

    for (auto& r: routesForDrawing)
    {
        r.Draw(doc);
    }

    for (auto& s: busTextNames)
    {
        s.Draw(doc);
    }

    for (auto& s: stopCircles)
    {
        s.Draw(doc);
    }

    std::vector<RouteText> stopTextNames{};
    for (auto stop: allStops)
    {
        stopTextNames.emplace_back(settings_.underlayer_color,
                                   settings_.underlayer_color,
                                   settings_.underlayer_width,
                                   sp(stop->coord),
                                   settings_.stop_label_offset,
                                   settings_.stop_label_font_size,
                                   "Verdana"s,
                                   stop->name);
        stopTextNames.emplace_back("black",
                                   sp(stop->coord),
                                   settings_.stop_label_offset,
                                   settings_.stop_label_font_size,
                                   "Verdana"s,
                                   stop->name);
    }

    for (auto& s: stopTextNames)
    {
        s.Draw(doc);
    }
}
