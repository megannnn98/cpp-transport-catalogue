#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "input_reader.h"
#include <ostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

namespace io {
class StatReader
{
    std::ostream& os_;
public:
    StatReader(const StatReader&) = delete;
    StatReader& operator=(const StatReader&) = delete;
    StatReader(StatReader&&) = delete;
    StatReader& operator=(StatReader&&) = delete;
    ~StatReader() = default;
    StatReader(std::ostream& os) : os_{os} {}

    using Bus = TransportCatalogue::Bus;
    using Stop = TransportCatalogue::Stop;

    void PrintBus(const TransportCatalogue& tc, std::string_view name) const
    {
        auto bus = tc.GetBus(name);
        auto stops = tc.GetBusStops(name);
        if (stops.empty()) {
            os_ << "Bus " << name << ": " << "not found" << std::endl;
            return;
        }

        auto uniqCalc = [](const std::vector<std::string_view>& stops) ->size_t {
            std::unordered_set<std::string_view> uniqueStops{};
            std::for_each(stops.cbegin(),
                          stops.cend(),
                          [&uniqueStops](const std::string_view stop){
                uniqueStops.insert(stop);
            });
            return uniqueStops.size();
        };

        double directDistance{};
        double realDistance{};
        double curvature{};
        std::vector<std::string_view>::iterator it = stops.begin();
        while (it != (stops.end() - 1)) {
            auto& coords1 = tc.GetStopCoords(*it);
            auto& coords2 = tc.GetStopCoords(*(std::next(it)));

            directDistance += ComputeDistance(coords1, coords2);

            const auto delta = tc.GetDistanceBetween(*it, *(std::next(it)));
            realDistance += delta ? delta : directDistance;

            it = std::next(it);
        }
        curvature = realDistance / directDistance;

//        Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
        os_ << "Bus " << bus << ": "
                  << stops.size() << " stops on route, "
                  << uniqCalc(stops) << " unique stops, "
                  << realDistance << " route length, "
                  << std::setprecision(6) << curvature << " curvature" << std::endl;
    }

    void PrintStop(const TransportCatalogue& tc, std::string_view name) const
    {
        auto& stops = tc.GetStops();
        auto it = std::find_if(stops.cbegin(),
                               stops.cend(),
                               [&name](const auto& p) {
                  return p.first == name;
              });
        if (it == stops.end()) {
            os_ << "Stop " << name << ": " << "not found" << std::endl;
            return;
        }

        if (it->second.second.empty()) {
            os_ << "Stop " << name << ": " << "no buses" << std::endl;
            return;
        }

        std::vector<std::string_view> vv{it->second.second.begin(),
                                         it->second.second.end()};

        std::sort(vv.begin(), vv.end());

        os_ << "Stop " << name << ": buses ";
        for (const auto& bus: vv)
        {
            os_ << bus;
            if (&bus != &vv.back()) {
                os_ << " ";
            }
        }
        os_ << std::endl;

    }
};

void inline ProcessRequest(const TransportCatalogue& tc, const InputReader& ir, const StatReader& sr)
{
    using namespace std::literals;
    static constexpr std::string_view BUS = "Bus"sv;
    static constexpr std::string_view STOP = "Stop"sv;
    InputReadParser parser{};
    std::string line{};
    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        parser.ltrim(line);
        parser.rtrim(line);
        if (line.length() && !line.find(BUS.data()))
        {
            sr.PrintBus(tc, line.substr(BUS.size() + 1, line.size() - (BUS.size() + 1)));
        }
        if (line.length() && !line.find(STOP.data()))
        {
            sr.PrintStop(tc, line.substr(STOP.size() + 1, line.size() - (STOP.size() + 1)));
        }
    } // end while
}
} // namespace io
