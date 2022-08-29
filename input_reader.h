#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <stack>
#include "transport_catalogue.h"

class InputReader
{
    TransportCatalogue& tc_;
public:
    InputReader(TransportCatalogue& tc) : tc_{tc} {}
    InputReader(const InputReader&) = delete;
    InputReader& operator=(const InputReader&) = delete;
    InputReader(InputReader&&) = default;
    InputReader& operator=(InputReader&&) = delete;
    ~InputReader() = default;

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

    std::string ReadLine() {
      std::string s{};
      std::getline(std::cin, s);
      return s;
    }

    int ReadLineWithNumber() {
      int result{};
      std::cin >> result;
      ReadLine();
      return result;
    }

    [[nodiscard]] TransportCatalogue::Bus ParseBus(std::string& busDataLine)
    {
        TransportCatalogue::Bus ret{};
        busDataLine = busDataLine.substr(4, busDataLine.length() - 4);

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
            ret.busStops.push_back(&tc_.GetStop(stopname));
            if (delim == '-') {
                circleStopHolder.push(&tc_.GetStop(stopname));
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
        static TransportCatalogue::Stop empty{};
        TransportCatalogue::Stop ret{};
        line = line.substr(5, line.length() - 5);

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

[[nodiscard]] TransportCatalogue Load(std::istream& input)
{
    TransportCatalogue ret{};
    InputReader ir{ret};
    std::string line{};
    std::vector<std::string> busDataLines{};

    std::getline(input, line);
    ir.ltrim(line);
    ir.rtrim(line);
    auto lineCnt = std::atoi(line.c_str());

    while (lineCnt--) {
        std::getline(input, line);
        ir.ltrim(line);
        ir.rtrim(line);
        if (line.length() <= 0) {
            continue;
        }
        if (line.find("Stop") == 0)
        {
            auto stop = ir.ParseStop(line);
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
        auto bus = ir.ParseBus(busDataLine);
        if (bus.busStops.empty()) {
            continue;
        }
        ret.AddBus(std::move(bus));
    }

    return ret;
}
