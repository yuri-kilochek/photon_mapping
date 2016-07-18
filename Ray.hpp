#ifndef PHOTON_MAPPING_RAY_HPP
#define PHOTON_MAPPING_RAY_HPP

#include "Point.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace photon_mapping {
    struct Ray {
        Point3f origin; Vector3f direction;

        Ray() {}

        Ray(Point3f const& origin, Vector3f const& direction)
            : origin(origin), direction(direction)
        {}
    };

    inline Ray operator*(Ray const& ray, Matrix43f const& matrix) {
        return Ray(ray.origin * matrix, unit(ray.direction * matrix));
    }

    inline Ray& operator*=(Ray& ray, Matrix43f const& matrix) {
        return ray = ray * matrix;
    }
}

#endif