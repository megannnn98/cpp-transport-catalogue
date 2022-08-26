#ifndef INPUTREADER_H
#define INPUTREADER_H


class InputReader
{
public:
    InputReader() = default;
    InputReader(const InputReader&) = default;
    InputReader& operator=(const InputReader&) = default;
    InputReader(InputReader&&) = default;
    InputReader& operator=(InputReader&&) = default;
    ~InputReader() = default;
};

#endif // INPUTREADER_H

//В первой строке стандартного потока ввода содержится
//число N — количество запросов на обновление базы данных,
//затем — по одному на строке — вводятся сами запросы. Запросы бывают двух типов.

//Stop X: latitude, longitude

//Bus X: описание маршрута
//    stop1 - stop2 - ... stopN:
//    stop1 > stop2 > ... > stopN > stop1
