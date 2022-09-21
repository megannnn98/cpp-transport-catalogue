#pragma once

#include <string>
#include "geo.h"

namespace domain {
struct Stop
{
    bool operator==(const domain::Stop& other) const;
    std::string name;
    geo::Coordinates coord;
};

struct Bus
{
    bool operator==(const domain::Bus& other) const;
    std::string name;
    bool isCircle{};
};

}// namespace domain
