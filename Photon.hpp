#ifndef PHOTON_MAPPING_PHOTON_HPP
#define PHOTON_MAPPING_PHOTON_HPP

#include "Point.hpp"
#include "Vector.hpp"
#include "Color.hpp"

namespace photon_mapping {
    struct Photon {
        Point3f position;
        Vector3f direction;
        Color power;
    };
}

#endif
