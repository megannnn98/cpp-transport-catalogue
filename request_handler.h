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
    const renderer::RenderSettings& renderer_{};
public:
    explicit RequestHandler(const TransportCatalogue& db, const renderer::RenderSettings& renderer)
        : db_{db}, renderer_{renderer}
    {
        (void)renderer_;
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

    class Route : public svg::Drawable {
        std::vector<svg::Point> points_{};
        double line_width_;
        svg::Color color_;

    public:
        ~Route() = default;
        explicit Route(std::vector<svg::Point>&& points, double line_width, svg::Color color)
            : points_{std::move(points)},
              line_width_{line_width},
              color_{color}
        {}

        void Draw(svg::ObjectContainer& container) const override {

            svg::Polyline pl{};
            for (auto l: points_) {
                pl.AddPoint(l);
            }
            pl.SetFillColor(svg::Color{});
            pl.SetStrokeWidth(line_width_);
            pl.SetStrokeColor(color_);
            pl.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            pl.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            container.Add(pl);
        }
    };

    class RouteText : public svg::Drawable {
        svg::Text text_{};
    public:
        ~RouteText() = default;
        explicit RouteText(svg::Color fill,
                           svg::Color stroke,
                           double stroke_width,
                           svg::Point pos,
                           svg::Point offset,
                           uint32_t font_size,
                           const std::string& font_family,
                           const std::string& font_weight,
                           const std::string& text)
        {
            text_.SetFillColor(fill)
                 .SetStrokeColor(stroke)
                 .SetStrokeWidth(stroke_width)
                 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                 .SetPosition(pos)
                 .SetOffset(offset)
                 .SetFontSize(font_size)
                 .SetFontFamily(font_family)
                 .SetFontWeight(font_weight)
                 .SetData(text);
        }

        explicit RouteText(svg::Color fill,
                           svg::Point pos,
                           svg::Point offset,
                           uint32_t font_size,
                           const std::string& font_family,
                           const std::string& font_weight,
                           const std::string& text)
        {
            text_.SetFillColor(fill)
                 .SetPosition(pos)
                 .SetOffset(offset)
                 .SetFontSize(font_size)
                 .SetFontFamily(font_family)
                 .SetFontWeight(font_weight)
                 .SetData(text);
        }



        explicit RouteText(svg::Color fill,
                           svg::Color stroke,
                           double stroke_width,
                           svg::Point pos,
                           svg::Point offset,
                           uint32_t font_size,
                           const std::string& font_family,
                           const std::string& text)
        {
            text_.SetFillColor(fill)
                 .SetStrokeColor(stroke)
                 .SetStrokeWidth(stroke_width)
                 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                 .SetPosition(pos)
                 .SetOffset(offset)
                 .SetFontSize(font_size)
                 .SetFontFamily(font_family)
                 .SetData(text);
        }

        explicit RouteText(svg::Color fill,
                           svg::Point pos,
                           svg::Point offset,
                           uint32_t font_size,
                           const std::string& font_family,
                           const std::string& text)
        {
            text_.SetFillColor(fill)
                 .SetPosition(pos)
                 .SetOffset(offset)
                 .SetFontSize(font_size)
                 .SetFontFamily(font_family)
                 .SetData(text);
        }
        void Draw(svg::ObjectContainer& container) const override {
            container.Add(text_);
        }
    };


    class StopCircle : public svg::Drawable {
        svg::Circle circle_;

    public:
        ~StopCircle() = default;
        explicit StopCircle(svg::Point pos, double radius, const std::string& fill)
            : circle_{}
        {
            circle_.SetCenter(pos)
                   .SetRadius(radius)
                   .SetFillColor(fill);
        }

        void Draw(svg::ObjectContainer& container) const override {
            container.Add(circle_);
        }
    };

    void MakeDoc(svg::Document& doc) const
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

        const auto& allBuses{db_.GetBuses()};
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

        renderer::SphereProjector sp(allCoords.begin(), allCoords.end(), renderer_.width, renderer_.height, renderer_.padding);

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
                        busTextNames.emplace_back(renderer_.underlayer_color,
                                                   renderer_.underlayer_color,
                                                   renderer_.underlayer_width,
                                                   stopPos,
                                                   renderer_.bus_label_offset,
                                                   renderer_.bus_label_font_size,
                                                   "Verdana"s,
                                                   "bold"s,
                                                   bus->name);
                        busTextNames.emplace_back(renderer_.color_palette.at(cnt),
                                                   stopPos,
                                                   renderer_.bus_label_offset,
                                                   renderer_.bus_label_font_size,
                                                   "Verdana"s,
                                                   "bold"s,
                                                   bus->name);
                }
                else if (!bus->isCircle &&
                         (i == ((stopNum-1)/2)) &&
                         (stopPos != sp(allBuses.at(bus->name).second.at(0)->coord)))
                {
                    busTextNames.emplace_back(renderer_.underlayer_color,
                                               renderer_.underlayer_color,
                                               renderer_.underlayer_width,
                                               stopPos,
                                               renderer_.bus_label_offset,
                                               renderer_.bus_label_font_size,
                                               "Verdana"s,
                                               "bold"s,
                                               bus->name);
                    busTextNames.emplace_back(renderer_.color_palette.at(cnt),
                                               stopPos,
                                               renderer_.bus_label_offset,
                                               renderer_.bus_label_font_size,
                                               "Verdana"s,
                                               "bold"s,
                                               bus->name);
                }

                points.emplace_back(stopPos);
            }


            Route route(std::move(points), renderer_.line_width, renderer_.color_palette.at(cnt));
            routesForDrawing.push_back(std::move(route));

            cnt++;
            cnt %= renderer_.color_palette.size();
        }

        std::vector<StopCircle> stopCircles{};
        for (auto stop: allStops)
        {
            stopCircles.emplace_back(sp(stop->coord), renderer_.stop_radius, "white"s);
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
            stopTextNames.emplace_back(renderer_.underlayer_color,
                                       renderer_.underlayer_color,
                                       renderer_.underlayer_width,
                                       sp(stop->coord),
                                       renderer_.stop_label_offset,
                                       renderer_.stop_label_font_size,
                                       "Verdana"s,
                                       stop->name);
            stopTextNames.emplace_back("black",
                                       sp(stop->coord),
                                       renderer_.stop_label_offset,
                                       renderer_.stop_label_font_size,
                                       "Verdana"s,
                                       stop->name);
        }

        for (auto& s: stopTextNames)
        {
            s.Draw(doc);
        }
    }
};

} // namespace tc
