#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include "transport_catalogue.h"

class InputReader
{
public:
    InputReader() = default;
    InputReader(const InputReader&) = default;
    InputReader& operator=(const InputReader&) = default;
    InputReader(InputReader&&) = default;
    InputReader& operator=(InputReader&&) = default;
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
};

//std::string prefix = "-param=";
//std::string argument = argv[1];
//if(argument.substr(0, prefix.size()) == prefix) {
//    std::string argumentValue = argument.substr(prefix.size());
//}
TransportCatalogue Load(std::istream& input)
{
    InputReader ir;
    std::string line{};
    TransportCatalogue::Stop stopHolder;
    TransportCatalogue::Bus busHolder;
    TransportCatalogue tc{};
    std::vector<std::string> busDataLines{};

    while (std::getline(input, line)) {
        ir.ltrim(line);
        ir.rtrim(line);
        const auto length = line.length();
        if (length > 0) {
            // "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
            if (line.find("Stop") == 0)
            {
                line = line.substr(5, line.length() - 5);

                auto semicon = line.rfind(':');
                if (semicon != std::string::npos)
                {
                    stopHolder.name = line.substr(0, semicon);
                    line = line.substr(semicon + 1);
                    auto comma = line.find(',');
                    if (comma != std::string::npos)
                    {
                        line[line.find(',')] = ' ';
                        std::stringstream ss{line};
                        float tmp{};
                        ss >> tmp;
                        stopHolder.coord.lat= tmp;
                        ss >> tmp;
                        stopHolder.coord.lng = tmp;

                        tc.AddBusStop(std::move(stopHolder));
                    }
                    else
                    {
                        assert(false);
                    }
                }
            }
            // "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
            else if (line.find("Bus") == 0)
            {
                busDataLines.push_back(std::move(line));
            }
        }
    }

    for(std::string& busDataLine: busDataLines )
    {
        busDataLine = busDataLine.substr(4, busDataLine.length() - 4);

        auto semicon = busDataLine.find(':');
        if (semicon != std::string::npos)
        {
            busDataLine[semicon] = ' ';
            char delim = (busDataLine.find('>') == std::string::npos) ? '-' : '>';
            std::stringstream ss{busDataLine};
            ss >> busHolder.name;

            std::string stopname{};
            while (std::getline(ss, stopname, delim))
            {
                ir.ltrim(stopname);
                ir.rtrim(stopname);
                busHolder.busStops.push_back(&tc.GetStop(stopname));
            }
            tc.AddBus(std::move(busHolder));
        }
    }

    return tc;
}

//В первой строке стандартного потока ввода содержится
//число N — количество запросов на обновление базы данных,
//затем — по одному на строке — вводятся сами запросы. Запросы бывают двух типов.

//Stop X: latitude, longitude

//Bus X: описание маршрута
//    stop1 - stop2 - ... stopN:
//    stop1 > stop2 > ... > stopN > stop1
