#pragma once

#include <algorithm>
#include <string>
#include <iostream>

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

    void Load(std::istream& input)
    {
        std::string line{};
        while (std::getline(input, line)) {
            ltrim(line);
            rtrim(line);
            const auto length = line.length();
            if (length > 0) {
            }
        }
    }
};

//В первой строке стандартного потока ввода содержится
//число N — количество запросов на обновление базы данных,
//затем — по одному на строке — вводятся сами запросы. Запросы бывают двух типов.

//Stop X: latitude, longitude

//Bus X: описание маршрута
//    stop1 - stop2 - ... stopN:
//    stop1 > stop2 > ... > stopN > stop1
