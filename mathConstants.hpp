#ifndef PHOTON_MAPPING_MATH_CONSTANTS_HPP
#define PHOTON_MAPPING_MATH_CONSTANTS_HPP

namespace photon_mapping {
    inline float nan() {
        union {
            int bits;
            float value;
        };
        bits = 0x7fffffff;
        return value;
    }

    template <typename T>
    inline bool nan(T x) {
        return x != x;
    }

    inline float inf() {
        union {
            int bits;
            float value;
        };
        bits = 0x7f800000;
        return value;
    }

    template <typename T>
    inline bool inf(T x) {
        return !nan(x) && nan(x - x);
    }

    inline float pi() {
        return 3.141592654f;
    }
}

#endif