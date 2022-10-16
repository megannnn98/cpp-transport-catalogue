#include "json_reader.h"
#include "json_builder.h"

json::JsonReader::JsonReader(std::istream& is)
    : is_{is}, doc_{json::Load(is_)}
{}

void json::JsonReader::ExtractStopsWithCoords(tc::TransportCatalogue& tc, const json::Array& arr)
{
    using namespace std::literals;
    auto extractStop = [](const json::Dict& dict) -> domain::Stop {
        return { dict.at("name"s).AsString(), {dict.at("latitude"s).AsDouble(), dict.at("longitude"s).AsDouble()}};
    };

    for (const auto& arr: arr) {

        auto it = arr.AsDict().find("type"s);
        if (it != arr.AsDict().cend() && it->second == "Stop"s)
        {
            domain::Stop stop{extractStop(arr.AsDict())};
            tc.AddStop(stop.name, stop.coord);
        }
    }
}

void json::JsonReader::ExtractBusWithStops(tc::TransportCatalogue& tc, const json::Array& arr)
{
    using namespace std::literals;
    auto extractBus = [](const json::Dict& dict, std::vector<std::string>& stopNames) -> domain::Bus {
        domain::Bus ret{dict.at("name"s).AsString(), dict.at("is_roundtrip"s).AsBool()};

        auto it = dict.find("stops"s);
        std::stack<std::string> circleStopHolder{};
        for (const auto& stop: it->second.AsArray()) {
            stopNames.push_back(stop.AsString());
            if (!ret.isCircle)
            {
                circleStopHolder.push(stop.AsString());
            }
        }
        if (!circleStopHolder.empty()) {
            circleStopHolder.pop();
            while (!circleStopHolder.empty()) {
                stopNames.push_back(circleStopHolder.top());
                circleStopHolder.pop();
            }
        }

        return ret;
    };

    for (const auto& arr: arr) {
        auto it = arr.AsDict().find("type"s);
        if (it != arr.AsDict().cend() && it->second == "Bus"s)
        {
            std::vector<std::string> stopNames{};
            domain::Bus bus{extractBus(arr.AsDict(), stopNames)};
            tc.AddBus(bus.name, stopNames, bus.isCircle);
        }
    }
}

void json::JsonReader::ExtractDistancesBetweenStops(tc::TransportCatalogue& tc, const json::Array& arr)
{
    using namespace std::literals;
    auto extractDistance = [](const json::Dict& dict, tc::TransportCatalogue::RetParseDistancesBetween& distances) {

        const std::string& from{dict.at("name"s).AsString()};
        const json::Dict& toStops{dict.at("road_distances"s).AsDict()};

        for (auto& [name, dist]: toStops) {
            distances.push_back(std::make_tuple(from, name, dist.AsInt()));
        }
    };

    for (const auto& arr: arr) {
        auto it = arr.AsDict().find("type"s);
        if (it != arr.AsDict().cend() && it->second == "Stop"s)
        {
            tc::TransportCatalogue::RetParseDistancesBetween distances{};
            extractDistance(arr.AsDict(), distances);
            tc.AddDistances(std::move(distances));
        }
    }
}

void json::JsonReader::AddBusesToStops(tc::TransportCatalogue& tc)
{
    for (auto& [busname, busStopsPair]: tc.GetBuses())
    {
        for (const domain::Stop* stop: busStopsPair.second )
        {
            tc.AddBusToStop(stop->name, busname);
        }
    }
}

tc::TransportCatalogue json::JsonReader::MakeCatalogue()
{
    using namespace std::literals;
    tc::TransportCatalogue tc{};

    const auto baseIt = doc_.GetRoot().AsDict().find("base_requests"s);
    if (baseIt == doc_.GetRoot().AsDict().cend()) {
        return tc;
    }

    ExtractStopsWithCoords(tc, baseIt->second.AsArray());
    ExtractBusWithStops(tc, baseIt->second.AsArray());
    ExtractDistancesBetweenStops(tc, baseIt->second.AsArray());
    AddBusesToStops(tc);

    return tc;
}

json::Node json::JsonReader::GenStop(int id, const tc::RequestHandler& handler, const json::Node& source) const
{
    using namespace std::literals;
    json::Dict dict{};
    dict["request_id"s] = id;
    std::string_view name = source.AsDict().at("name"s).AsString();

    if (handler.GetStop(name).name.empty())
    {
        return json::Builder{}.
                StartDict().Key("error_message").Value("not found"s).EndDict().
                Build();
    }
    else
    {
        json::Array arrBuses{};
        auto buses = handler.GetBusesFromStop(name);
        for (const auto& b: buses)
        {
            arrBuses.push_back(b->name);
        }
        std::sort(arrBuses.begin(), arrBuses.end(), [](json::Node& a, json::Node& b){
            return std::lexicographical_compare(a.AsString().begin(), a.AsString().end(),
                                          b.AsString().begin(), b.AsString().end());
        });

        return json::Builder{}.
                StartDict().Key("buses").Value(std::move(arrBuses)).EndDict().
                Build();
    }
}

json::Node json::JsonReader::GenBus(int id, const tc::RequestHandler& handler, const json::Node& source) const
{
    using namespace std::literals;
    std::string_view name = source.AsDict().at("name"s).AsString();
    json::Dict dict{};
    auto bus = handler.GetBus(name);

    dict["request_id"s] = id;
    if (bus.name.empty())
    {
        return json::Builder{}.
              StartDict().Key("error_message").Value("not found"s).EndDict().
              Build();
    }
    else
    {
        auto uniqCalc = [](const std::vector<const domain::Stop *> &stops) -> size_t {
          std::unordered_set<std::string_view> uniqueStops{};
          std::for_each(
              stops.cbegin(), stops.cend(),
              [&uniqueStops](const domain::Stop *stop) { uniqueStops.insert(stop->name); });
          return uniqueStops.size();
        };

        double directDistance{};
        double realDistance{};
        auto stops = handler.GetStopsFromBus(name);

        std::vector<const domain::Stop *>::const_iterator it = stops.cbegin();
        while (it != (stops.cend() - 1)) {
          auto nameA = (*it)->name;
          auto nameB = (*std::next(it))->name;
          directDistance +=
              ComputeDistance(handler.GetStopCoords(nameA), handler.GetStopCoords(nameB));

          const auto delta = handler.GetDistanceBetween(nameA, nameB);
          realDistance += delta ? delta : directDistance;

          it = std::next(it);
        }

        return json::Builder{}.
                StartDict().
                    Key("curvature").Value(realDistance / directDistance).
                    Key("route_length").Value(realDistance).
                    Key("stop_count").Value(static_cast<int>(stops.size())).
                    Key("unique_stop_count").Value(static_cast<int>(uniqCalc(stops))).
                EndDict().
                Build();
    }

    return dict;
}

json::Node json::JsonReader::GenMap(int id, const tc::RequestHandler& handler) const
{
    using namespace std::literals;
    svg::Document doc;
    handler.MakeDoc(doc);
    std::ostringstream stream;
    doc.Render(stream);

    return Builder{}.StartDict()
                        .Key("map"s).Value(stream.str())
                        .Key("request_id"s).Value(id)
                    .EndDict().Build();
}

void json::JsonReader::Print(const tc::RequestHandler& handler, std::ostream& os) const
{
    using namespace std::literals;
    const auto baseIt = doc_.GetRoot().AsDict().find("stat_requests"s);
    if (baseIt == doc_.GetRoot().AsDict().cend()) {
        return;
    }

    const auto& requests = baseIt->second.AsArray();

    json::Array mainArr{};
    for (const auto& arr: requests) {
        std::string_view type = arr.AsDict().at("type"s).AsString();
        int id = arr.AsDict().at("id"s).AsInt();

        if (type == "Stop"s) {
            mainArr.push_back(std::move(GenStop(id, handler, arr)));
        }
        else if (type == "Bus"s) {
            mainArr.push_back(std::move(GenBus(id, handler, arr)));
        }
        else if (type == "Map"s) {
            mainArr.push_back(std::move(GenMap(id, handler)));
        }
    } // end for
    json::Print(json::Document{mainArr}, os);
}

renderer::RenderSettings json::JsonReader::MakeRenderSettings()
{
    renderer::RenderSettings rs{};
    using namespace std::literals;
    const auto baseIt = doc_.GetRoot().AsDict().find("render_settings"s);
    if (baseIt == doc_.GetRoot().AsDict().cend()) {
        return rs;
    }

    const json::Dict& settings_map = baseIt->second.AsDict();
    rs.width = settings_map.at("width"s).AsDouble();
    rs.height = settings_map.at("height"s).AsDouble();
    rs.padding = settings_map.at("padding"s).AsDouble();
    rs.stop_radius = settings_map.at("stop_radius"s).AsDouble();
    rs.line_width = settings_map.at("line_width"s).AsDouble();
    rs.bus_label_font_size = settings_map.at("bus_label_font_size"s).AsInt();
    const json::Array& bus_label_offset = settings_map.at("bus_label_offset"s).AsArray();
    rs.bus_label_offset = { bus_label_offset[0].AsDouble(),
                          bus_label_offset[1].AsDouble() };
    rs.stop_label_font_size = settings_map.at("stop_label_font_size"s).AsInt();
    const json::Array& stop_label_offset = settings_map.at("stop_label_offset"s).AsArray();
    rs.stop_label_offset = { stop_label_offset[0].AsDouble(),
                           stop_label_offset[1].AsDouble() };
    if (settings_map.at("underlayer_color"s).IsArray()) {
        const json::Array& arr = settings_map.at("underlayer_color"s).AsArray();
        if (arr.size() == 3) {
            svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
            rs.underlayer_color = rgb_colors;
        }
        else if (arr.size() == 4) {
            svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
            rs.underlayer_color = rgba_colors;
        }
        else throw std::logic_error("Strange array"s);
    }
    else if (settings_map.at("underlayer_color"s).IsString()) {
        rs.underlayer_color = settings_map.at("underlayer_color"s).AsString();
    }
    else throw std::logic_error("Error color identity"s);
    rs.underlayer_width = settings_map.at("underlayer_width"s).AsDouble();
    const json::Array& color_palette = settings_map.at("color_palette"s).AsArray();
    for (const json::Node& node : color_palette) {
        if (node.IsArray()) {
            const json::Array& arr = node.AsArray();
            if (arr.size() == 3) {
                svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                rs.color_palette.push_back(rgb_colors);
            }
            else if (arr.size() == 4) {
                svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                rs.color_palette.push_back(rgba_colors);
            }
            else throw std::logic_error("Strange array"s);
        }
        else if (node.IsString()) {
            rs.color_palette.push_back(node.AsString());
        }
        else throw std::logic_error("Error palette color identity"s);
    }
    return rs;
}
