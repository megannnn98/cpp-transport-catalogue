#include "geo.h"
#include <cmath>

bool geo::Coordinates::operator==(const Coordinates& other) const {
    return lat == other.lat && lng == other.lng;
}
bool geo::Coordinates::operator!=(const Coordinates& other) const {
    return !(*this == other);
}


double geo::ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    static constexpr const double dr = 3.1415926535 / 180.;
    static constexpr const double EARTH_RADIUS = 6371000.;

    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * EARTH_RADIUS;
}
