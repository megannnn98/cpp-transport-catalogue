#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

using namespace std::literals;

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

struct Point {
  Point() = default;
  Point(double x, double y) : x(x), y(y) {}
  double x = 0;
  double y = 0;

  bool operator==(const Point& other) const
  {
      return (x == other.x) && (y == other.y);
  }

  bool operator!=(const Point& other) const
  {
      return !(*this == other);
  }
};

struct Rgb
{
    std::uint8_t red{};
    std::uint8_t green{};
    std::uint8_t blue{};

    Rgb() = default;
    Rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
        : red{r}, green{g}, blue{b}
    {
    }
};

struct Rgba
{
    std::uint8_t red{};
    std::uint8_t green{};
    std::uint8_t blue{};
    double opacity{1.};

    Rgba() = default;
    Rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, double o)
        : red{r}, green{g}, blue{b}, opacity{o}
    {
    }
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"s};

struct ColorPrinter {

    std::ostream& os;
    void operator()(std::monostate) const {
        os << "none"sv;
    }
    void operator()(std::string color) const {
        os << color;
    }
    void operator()(Rgb color) const {
        os << "rgb("sv
           << static_cast<int>(color.red) << ","
           << static_cast<int>(color.green) << ","
           << static_cast<int>(color.blue) << ")";
    }
    void operator()(Rgba color) const {
        os << "rgba("sv
           << static_cast<int>(color.red) << ","
           << static_cast<int>(color.green) << ","
           << static_cast<int>(color.blue) << ","
           << color.opacity << ")";
    }
};

inline std::ostream& operator<<(std::ostream& os, Color color) {
    std::visit(ColorPrinter{os}, color);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, StrokeLineCap slc)
{
    switch (slc) {
        case StrokeLineCap::BUTT:
            os << "butt"sv;
        break;
        case StrokeLineCap::ROUND:
            os << "round"sv;
        break;
        case StrokeLineCap::SQUARE:
            os << "square"sv;
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, StrokeLineJoin slj)
{
    switch (slj) {
        case StrokeLineJoin::ARCS:
            os << "arcs"sv;
        break;
        case StrokeLineJoin::BEVEL:
            os << "bevel"sv;
        break;
        case StrokeLineJoin::MITER:
            os << "miter"sv;
        break;
        case StrokeLineJoin::MITER_CLIP:
            os << "miter-clip"sv;
        break;
        case StrokeLineJoin::ROUND:
            os << "round"sv;
        break;
    }
    return os;
}

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }
    Owner& SetStrokeWidth(double stroke_width) {
        stroke_width_ = stroke_width;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {

        if (fill_color_.has_value()) {
            out << " fill=\""sv;
            std::visit(ColorPrinter{out}, fill_color_.value());
            out << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv;
            std::visit(ColorPrinter{out}, stroke_color_.value());
            out << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
    std::optional<double> stroke_width_;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с
 * отступами. Хранит ссылку на поток вывода, текущее значение и шаг отступа при
 * выводе элемента
 */
struct RenderContext {
  RenderContext(std::ostream &out) : out(out) {}

  RenderContext(std::ostream &out, int indent_step, int indent = 0)
      : out(out), indent_step(indent_step), indent(indent) {}

  RenderContext Indented() const {
    return {out, indent_step, indent + indent_step};
  }

  void RenderIndent() const {
    for (int i = 0; i < indent; ++i) {
      out.put(' ');
    }
  }

  std::ostream &out;
  int indent_step = 0;
  int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
  void Render(const RenderContext &context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
  }

  virtual ~Object() = default;

private:
  virtual void RenderObject(const RenderContext &context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
  Circle() = default;
  virtual ~Circle() = default;
  Circle &SetCenter(Point center) {
    center_ = center;
    return *this;
  }

  Circle &SetRadius(double radius) {
    radius_ = radius;
    return *this;
  }

private:
  virtual void RenderObject(const RenderContext &context) const override {

    auto &out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
  }

  Point center_;
  double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline> {
public:
  Polyline() = default;
  virtual ~Polyline() = default;
  // Добавляет очередную вершину к ломаной линии
  Polyline &AddPoint(Point point) {
    points_.push_back(std::move(point));
    return *this;
  }

private:
  virtual void RenderObject(const RenderContext &context) const override {
    auto &out = context.out;
    // <polyline
    // points="20,40 22.9389,45.9549 29.5106,46.9098 24.7553,51.5451 25.8779,58.0902
    // 20,55 14.1221,58.0902 15.2447,51.5451 10.4894,46.9098 17.0611,45.9549
    // 20,40" />
    out << "<polyline points=\""sv;
    std::size_t i{points_.size()};
    for (auto &[x, y] : points_) {
      out << x << ","sv << y;
      if (--i) {
        out << " "sv;
      }
    }
    out << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
  }

  std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {
public:
  Text() = default;
  virtual ~Text() = default;
  // Задаёт координаты опорной точки (атрибуты x и y)
  Text &SetPosition(Point pos) {
    pos_ = pos;
    return *this;
  }

  // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
  Text &SetOffset(Point offset) {
    offset_ = offset;
    return *this;
  }

  // Задаёт размеры шрифта (атрибут font-size)
  Text &SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
  }

  // Задаёт название шрифта (атрибут font-family)
  Text &SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
  }

  // Задаёт толщину шрифта (атрибут font-weight)
  Text &SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
  }

  // Задаёт текстовое содержимое объекта (отображается внутри тега text)
  Text &SetData(std::string data) {
    data_ = std::move(data);
    return *this;
  }

private:
  virtual void RenderObject(const RenderContext &context) const override {
    // <text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
    auto &out = context.out;
    out << "<text "sv;
    RenderAttrs(out);
    out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
    out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
    out << "font-size=\""sv << font_size_ << "\" "sv;
    if (!font_family_.empty())
        out << "font-family=\""sv << font_family_ << "\" "sv;
    if (!font_weight_.empty())
        out << "font-weight=\""sv << font_weight_ << "\""sv;
    out << ">" << data_ << "</text>"sv;
  }

  Point pos_{};
  Point offset_{};
  std::uint32_t font_size_{1UL};
  std::string font_family_{};
  std::string font_weight_{};
  std::string data_{};
};

class ObjectContainer
{
public:
    virtual ~ObjectContainer() = default;
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }
};

class Document: public ObjectContainer {
    std::vector<std::unique_ptr<Object>> objects_;
public:
    virtual ~Document() = default;
    void AddPtr(std::unique_ptr<Object> &&obj) override {
      objects_.push_back(std::move(obj));
    }

    void Render(std::ostream &out) const {
      out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
      out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv
          << std::endl;
      for (auto &element : objects_) {
        element->Render(out);
      }
      out << "</svg>"sv;
    }
};

class Drawable
{
public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& ) const = 0;
};


} // namespace svg
