
#include "domain.h"

bool domain::Stop::operator==(const domain::Stop& other) const
{
    return (name == other.name);
}

bool domain::Bus::operator==(const domain::Bus& other) const
{
    return (name == other.name);
}
