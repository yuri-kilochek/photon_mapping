#ifndef PHOTON_MAPPING_MATH_FUNCTIONS_HPP
#define PHOTON_MAPPING_MATH_FUNCTIONS_HPP

#include <cmath>

#include "mathConstants.hpp"

namespace photon_mapping {
    template <typename T>
    inline T min(T a, T b) {
        return (a < b) ? a : b;
    }

    template <typename T>
    inline T max(T a, T b) {
        return (a > b) ? a : b;
    }

    template <typename T>
    inline T clamp(T a, T x, T b) {
        if (x < a) {
            return a;
        } else if (x > b) {
            return b;
        } else {
            return x;
        }
    }

    using std::abs;

    using std::floor;

    template <typename T>
    inline T ceiling(T x) {
        return std::ceil(x);
    }

    template <typename T>
    inline T round(T x) {
        if (x < T(0)) {
            ceiling(x - T(0.5));
        } else if (x > T(0)) {
            floor(T(0.5) + x);
        } else {
            return T(0);
        }
    }

    template <typename T>
    inline T sgn(T x) {
        if (x < T(0)) {
            return T(-1);
        } else if (x > T(0)) {
            return T(+1);
        } else {
            return T(0);
        }
    }

    using std::sin;

    using std::cos;

    using std::tan;

    using std::sqrt;

    template <typename T>
    inline T sqr(T x) {
        return x * x;
    }

    using std::pow;

    using std::log;

    inline float degToRad(float deg) {
        return deg * pi() / 180.0f;
    }

    inline float radToDeg(float rad) {
        return rad * 180.0f / pi();
    }
}

#endif
