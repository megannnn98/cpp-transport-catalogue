#pragma once

#include <vector>
#include <algorithm>
#include <set>

#include "svg.h"
#include "geo.h"
#include "domain.h"

namespace renderer
{
using namespace std::string_literals;

inline const double EPSILON = 1e-6;
inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct RenderSettings
{

  double width{};
  double height{};

  double padding{};

  double line_width{};
  double stop_radius{};

  int bus_label_font_size{};
  svg::Point bus_label_offset{};

  int stop_label_font_size{};
  svg::Point stop_label_offset{};

  svg::Color underlayer_color{};
  double underlayer_width{};

  std::vector<svg::Color> color_palette{};

};

struct MapRenderer
{
    RenderSettings settings_{};

    explicit MapRenderer(RenderSettings settings)
        : settings_{settings}
    {
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

    void MakeDoc(svg::Document& doc, const std::unordered_map<std::string_view, std::pair<domain::Bus*, std::vector<const domain::Stop*>>>& allBuses) const;
};



} // renderer
