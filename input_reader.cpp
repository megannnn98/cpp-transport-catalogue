#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include <string_view>
#include <cassert>
#include "input_reader.h"
#include "transport_catalogue.h"

namespace tc {
namespace io {

[[nodiscard]] std::string InputReader::ReadLine() const {
    std::string s{};
    std::getline(is_, s);
    return s;
}

[[nodiscard]] int InputReader::ReadLineWithNumber() const {
    int result{};
    is_ >> result;
    std::ignore = ReadLine();
    return result;
}

void InputReadParser::ltrim(std::string& s) {
    s.erase(s.begin(),
                std::find_if(s.begin(), s.end(),
                                [](char ch) { return !std::isspace(ch); }));
}

void InputReadParser::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                                [](char ch) { return !std::isspace(ch); }).base(),
                s.end());
}

[[nodiscard]] TransportCatalogue::RetParseBus InputReadParser::ParseBus(std::string_view busDataLine)
{
    using namespace std::literals;
    static constexpr std::string_view BUS = "Bus"sv;
    TransportCatalogue::RetParseBus ret{};
    busDataLine = busDataLine.substr(BUS.size() + 1, busDataLine.length() - (BUS.size() + 1));

    auto colon = busDataLine.find_first_of(':');
    if (colon == std::string::npos)
    {
        assert(false);
        static std::tuple<std::string, std::vector<std::string>, bool> empty{};
        return empty;
    }

    std::get<0>(ret) = busDataLine.substr(0, colon);
    busDataLine = busDataLine.substr(colon + 2, busDataLine.size() - (colon + 2));
    char delim = (busDataLine.find('>') == std::string::npos) ? '-' : '>';
    std::get<2>(ret) = (delim == '>');
    std::stringstream ss{std::string{busDataLine}};

    std::string stopname{};
    std::stack<std::string> circleStopHolder{};
    while (std::getline(ss, stopname, delim))
    {
        ltrim(stopname);
        rtrim(stopname);
        std::get<1>(ret).push_back(stopname);
        if (delim == '-') {
            circleStopHolder.push(stopname);
        }
    }
    if (!circleStopHolder.empty()) {
        circleStopHolder.pop();
        while (!circleStopHolder.empty()) {
            std::get<1>(ret).push_back(circleStopHolder.top());
            circleStopHolder.pop();
        }
    }

    return ret;
}

[[nodiscard]] TransportCatalogue::RetParseStop InputReadParser::ParseStop(std::string& line)
{
    using namespace std::literals;
    static constexpr std::string_view STOP = "Stop"sv;
    static TransportCatalogue::RetParseStop emptyStop{};
    TransportCatalogue::RetParseStop ret{};
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


    if (comma = line.find(',');
        std::string::npos == comma) {
        line.clear();
    }
    else
    {
        line = ret.first + ": "s + line.substr(comma + 1);
    }

    return ret;
}

[[nodiscard]] TransportCatalogue::RetParseDistancesBetween InputReadParser::ParseDistancesBetween(std::string_view line)
{
    using namespace std::literals;
    static constexpr std::string_view TO = "to "sv;
    static constexpr const unsigned MIN_LINE_LEN = 2U;
    TransportCatalogue::RetParseDistancesBetween ret{};
    std::string dataLine{};
    const std::string nameA{line.substr(0, line.find_first_of(':'))};
    std::stringstream ss{std::string{line.substr(nameA.size() + 2)}};

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

        ret.push_back(std::move(el));
    }
    return ret;
}

[[nodiscard]] TransportCatalogue Load(const InputReader& ir)
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
            if (!line.empty()) {
                stopDataLines.push_back(std::move(line));
            }
            tc.AddStop(tmp.first, tmp.second);
        }
        else if ((line.find("Bus") == 0) && (line.find(':') != std::string::npos))
        {
            busDataLines.push_back(std::move(line));
        }
    } // end while

    auto processBusLines = [&busDataLines, &irp, &tc]{
        for(std::string& busDataLine: busDataLines )
        {
            TransportCatalogue::RetParseBus tmp{irp.ParseBus(busDataLine)};
            tc.AddBus(std::get<0>(tmp), std::get<1>(tmp), std::get<2>(tmp));
        }
    };

    auto addBusesToStops = [&tc]{

        // std::unordered_map<std::string_view, std::pair<Bus*, StopPointersContainer>> busnameToBus_{};
        for (auto& [busname, busStopsPair]: tc.GetBuses())
        {
            // using StopPointersContainer = std::vector<const Stop*>;
            for (const TransportCatalogue::Stop* stop: busStopsPair.second )
            {
                tc.AddBusToStop(stop->name, busname);
            }
        }
    };

    auto addDistancesBetweenStops = [&stopDataLines, &irp, &tc]{
        for(std::string& stopDataLine: stopDataLines )
        {
            tc.AddDistances(irp.ParseDistancesBetween(stopDataLine));
        }
    };

    processBusLines();
    addBusesToStops();
    addDistancesBetweenStops();

    if (tc.GetBuses().empty() || tc.GetStops().empty())
        std::runtime_error("buses or stops can't be empty");

    return tc;
}

}// namespace tc
}// namespace io
