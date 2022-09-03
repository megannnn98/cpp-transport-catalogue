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

        auto uniqCalc = [](const std::vector<const Stop*>& stops) ->size_t {
            std::unordered_set<std::string_view> uniqueStops{};
            std::for_each(stops.cbegin(),
                          stops.cend(),
                          [&uniqueStops](const Stop* stop){
                uniqueStops.insert(stop->name);
            });
            return uniqueStops.size();
        };

        double directDistance{};
        double realDistance{};
        double curvature{};

        std::vector<const Stop*>::const_iterator it = stops.cbegin();
        while (it != (stops.cend() - 1)) {
            auto nameA = (*it)->name;
            auto nameB = (*std::next(it))->name;
            directDistance += ComputeDistance(tc.GetStopCoords(nameA), tc.GetStopCoords(nameB));

            const auto delta = tc.GetDistanceBetween(nameA, nameB);
            realDistance += delta ? delta : directDistance;

            it = std::next(it);
        }
        curvature = realDistance / directDistance;

//        Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
        os_ << "Bus " << name << ": "
                  << stops.size() << " stops on route, "
                  << uniqCalc(stops) << " unique stops, "
                  << realDistance << " route length, "
                  << std::setprecision(6) << curvature << " curvature" << std::endl;
    }

    void PrintStop(const TransportCatalogue& tc, std::string_view name) const
    {
        // std::unordered_map<std::string_view, std::pair<Stop*, BusPointersContainer>> stopnameToStop_{};
        auto& stops = tc.GetStops();
        if (!stops.count(name)) {
            os_ << "Stop " << name << ": " << "not found" << std::endl;
            return;
        }

        if (stops.at(name).second.empty()) {
            os_ << "Stop " << name << ": " << "no buses" << std::endl;
            return;
        }

        std::vector<std::string_view> vv(stops.at(name).second.size());

        int i = 0;
        for (auto v: stops.at(name).second)
        {
            vv[i++] = v->name;
        }

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
