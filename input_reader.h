#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include <string_view>
#include "transport_catalogue.h"

namespace input {
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

    [[nodiscard]] TransportCatalogue::Bus ParseBus(TransportCatalogue& tc, std::string& busDataLine)
    {
        using namespace std::literals;
        static constexpr std::string_view BUS = "Bus"sv;
        TransportCatalogue::Bus ret{};
        busDataLine = busDataLine.substr(BUS.size() + 1, busDataLine.length() - BUS.size() + 1);

        auto semicon = busDataLine.find(':');
        if (semicon == std::string::npos)
        {
            static TransportCatalogue::Bus empty{};
            return empty;
        }

        busDataLine[semicon] = ' ';
        char delim = (busDataLine.find('>') == std::string::npos) ? '-' : '>';
        std::stringstream ss{busDataLine};
        ss >> ret.name;

        std::string stopname{};
        std::stack<TransportCatalogue::Stop*> circleStopHolder{};
        while (std::getline(ss, stopname, delim))
        {
            ltrim(stopname);
            rtrim(stopname);
            ret.busStops.push_back(&tc.GetStop(stopname));
            if (delim == '-') {
                circleStopHolder.push(&tc.GetStop(stopname));
            }
        }
        if (!circleStopHolder.empty()) {
            circleStopHolder.pop();
            while (!circleStopHolder.empty()) {
                ret.busStops.push_back(circleStopHolder.top());
                circleStopHolder.pop();
            }
        }

        return ret;
    }

    [[nodiscard]] TransportCatalogue::Stop ParseStop(std::string& line)
    {
        using namespace std::literals;
        static constexpr std::string_view STOP = "Stop"sv;
        static TransportCatalogue::Stop empty{};
        TransportCatalogue::Stop ret{};
        line = line.substr(STOP.size() + 1, line.length() - STOP.size() + 1);

        auto semicon = line.rfind(':');
        if (semicon == std::string::npos) {
            return empty;
        }

        ret.name = line.substr(0, semicon);
        line = line.substr(semicon + 1);
        auto comma = line.find(',');
        if (comma == std::string::npos) {
            return empty;
        }
        line[line.find(',')] = ' ';
        std::stringstream ss{line};
        float tmp{};
        ss >> tmp;
        ret.coord.lat= tmp;
        ss >> tmp;
        ret.coord.lng = tmp;

        return ret;
    }

};

[[nodiscard]] inline TransportCatalogue Load(InputReader& ir)
{
    TransportCatalogue ret{};
    InputReadParser irp{};
    std::string line{};
    std::vector<std::string> busDataLines{};

    auto lineCnt = ir.ReadLineWithNumber();

    while (lineCnt--) {
        line = ir.ReadLine();
        irp.ltrim(line);
        irp.rtrim(line);
        if (line.length() <= 0) {
            continue;
        }
        if (line.find("Stop") == 0)
        {
            auto stop = irp.ParseStop(line);
            if (stop.name.empty()) {
                continue;
            }
            ret.AddBusStop(std::move(stop));
        }
        else if ((line.find("Bus") == 0) && (line.find(':') != std::string::npos))
        {
            busDataLines.push_back(std::move(line));
        }
    } // end while

    for(std::string& busDataLine: busDataLines )
    {
        auto bus = irp.ParseBus(ret, busDataLine);
        if (bus.busStops.empty()) {
            continue;
        }
        ret.AddBus(std::move(bus));
    }

    return ret;
}

} // namespace input
