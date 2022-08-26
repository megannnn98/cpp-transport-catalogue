#pragma once

class StatReader
{
public:
    StatReader() = default;
    StatReader(const StatReader&) = default;
    StatReader& operator=(const StatReader&) = default;
    StatReader(StatReader&&) = default;
    StatReader& operator=(StatReader&&) = default;
    ~StatReader() = default;
};

