#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include <string_view>
#include <cassert>
#include "transport_catalogue.h"

namespace io {
class InputReader
{
    std::istream& is_;
public:
    InputReader(std::istream& is) : is_{is} {}
    InputReader(const InputReader&) = delete;
    InputReader& operator=(const InputReader&) = delete;
    InputReader(InputReader&&) = delete;
    InputReader& operator=(InputReader&&) = delete;
    ~InputReader() = default;

    [[nodiscard]] std::string ReadLine() const {
      std::string s{};
      std::getline(is_, s);
      return s;
    }

    [[nodiscard]] int ReadLineWithNumber() const {
      int result{};
      is_ >> result;
      std::ignore = ReadLine();
      return result;
    }
};

class InputReadParser
{
public:
    InputReadParser() = default;
    InputReadParser(const InputReadParser&) = delete;
    InputReadParser& operator=(const InputReadParser&) = delete;
    InputReadParser(InputReadParser&&) = delete;
    InputReadParser& operator=(InputReadParser&&) = delete;
    ~InputReadParser() = default;

    void ltrim(std::string& s) {
        s.erase(s.begin(),
                    std::find_if(s.begin(), s.end(),
                                 [](char ch) { return !std::isspace(ch); }));
    };

    void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                                 [](char ch) { return !std::isspace(ch); }).base(),
                    s.end());
    };

    [[nodiscard]] TransportCatalogue::RetParseBus ParseBus(std::string& busDataLine)
    {
        using namespace std::literals;
        static constexpr std::string_view BUS = "Bus"sv;
        std::pair<std::string, std::vector<std::string>> ret{};
        busDataLine = busDataLine.substr(BUS.size() + 1, busDataLine.length() - (BUS.size() + 1));

        auto colon = busDataLine.find_first_of(':');
        if (colon == std::string::npos)
        {
            assert(false);
            static std::pair<std::string, std::vector<std::string>> empty{};
            return empty;
        }

        ret.first = busDataLine.substr(0, colon);
        busDataLine = busDataLine.substr(colon + 2, busDataLine.size() - (colon + 2));
        char delim = (busDataLine.find('>') == std::string::npos) ? '-' : '>';
        std::stringstream ss{busDataLine};

        std::string stopname{};
        std::stack<std::string> circleStopHolder{};
        while (std::getline(ss, stopname, delim))
        {
            ltrim(stopname);
            rtrim(stopname);
            ret.second.push_back(stopname);
            if (delim == '-') {
                circleStopHolder.push(stopname);
            }
        }
        if (!circleStopHolder.empty()) {
            circleStopHolder.pop();
            while (!circleStopHolder.empty()) {
                ret.second.push_back(circleStopHolder.top());
                circleStopHolder.pop();
            }
        }

        return ret;
    }

    [[nodiscard]] TransportCatalogue::RetParseStop ParseStop(std::string& line)
    {
        using namespace std::literals;
        static constexpr std::string_view STOP = "Stop"sv;
        static TransportCatalogue::RetParseStop emptyStop{};
        std::pair<std::string, geo::Coordinates> ret{};
        line = line.substr(STOP.size() + 1, line.length() - (STOP.size() + 1));

        auto semicon = line.find_first_of(':');
        if (semicon == std::string::npos) {
            assert(false);
            return emptyStop;
        }

        ret.first = line.substr(0, semicon);
        line = line.substr(semicon + 1);
        auto comma = line.find_first_of(',');
        if (comma == std::string::npos) {
            assert(false);
            return emptyStop;
        }
        line[comma] = ' ';

        std::stringstream ss{line};
        ss >> ret.second.lat >> ret.second.lng;

        line = ret.first + ": "s + line.substr(line.find(',') + 1);

        return ret;
    }

    [[nodiscard]] TransportCatalogue::RetParseDistancesBetween ParseDistancesBetween(std::string& line)
    {
        using namespace std::literals;
        static constexpr std::string_view TO = "to "sv;
        static constexpr const unsigned MIN_LINE_LEN = 2U;
        TransportCatalogue::RetParseDistancesBetween ret{};
        std::string dataLine{};
        std::string nameA{line.substr(0, line.find_first_of(':'))};
        line = line.substr(nameA.size() + 2);
        std::stringstream ss{line};

        while (std::getline(ss, dataLine, ','))
        {
            if (dataLine.size() < MIN_LINE_LEN )
                continue;
            ltrim(dataLine);
            rtrim(dataLine);
            TransportCatalogue::RetParseDistancesBetweenElement el{};

            std::get<2>(el) = std::stoul(dataLine.substr(0, dataLine.find('m')));
            std::get<1>(el) = dataLine.substr(dataLine.find(TO.data()) + TO.size());
            std::get<0>(el) = nameA;

            ret.push_back(el);
        }
        return ret;
    }
};

[[nodiscard]] inline TransportCatalogue Load(const InputReader& ir)
{
    TransportCatalogue tc{};
    InputReadParser irp{};
    std::string line{};
    std::vector<std::string> busDataLines{};
    std::vector<std::string> stopDataLines{};

    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        irp.ltrim(line);
        irp.rtrim(line);

        if (line.find("Stop") == 0)
        {
            TransportCatalogue::RetParseStop tmp{irp.ParseStop(line)};
            stopDataLines.push_back(std::move(line));
            tc.AddStop(std::move(tmp));
        }
        else if ((line.find("Bus") == 0) && (line.find(':') != std::string::npos))
        {
            busDataLines.push_back(std::move(line));
        }
    } // end while

    auto busLinesProcessing = [&busDataLines, &irp, &tc]{
        for(std::string& busDataLine: busDataLines )
        {
            TransportCatalogue::RetParseBus tmp{irp.ParseBus(busDataLine)};
            tc.AddBusAndStops(TransportCatalogue::Bus{tmp.first}, std::move(tmp.second));
        }
    };

    auto addBusesToStops = [&tc]{
        for (auto& busPair: tc.GetBuses())
        {
            std::vector<std::string_view>& stops = busPair.second.second;
            for (std::string_view stopName: stops )
            {
                tc.AddBusToStop(stopName, busPair.first);
            }
        }
    };

    auto addDistancesBetweenStops = [&stopDataLines, &irp, &tc]{
        for(std::string& stopDataLine: stopDataLines )
        {
            tc.AddDistances(irp.ParseDistancesBetween(stopDataLine));
        }
    };

    busLinesProcessing();
    addBusesToStops();
    addDistancesBetweenStops();

    if (tc.GetBuses().empty() || tc.GetStops().empty())
        std::runtime_error("buses or stops can't be empty");

    return tc;
}

} // namespace io
