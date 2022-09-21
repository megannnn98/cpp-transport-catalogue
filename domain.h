#pragma once

#include "geo.h"
#include <string>

namespace domain {
struct Stop
{
    bool operator==(const domain::Stop& other) const
    {
        return (name == other.name);
    }

    std::string name;
    geo::Coordinates coord;
};

struct Bus
{
    bool operator==(const domain::Bus& other) const
    {
        return (name == other.name);
    }

    std::string name;
    bool isCircle{};
};

}
