#ifndef PHOTON_MAPPING_EXTENT_HPP
#define PHOTON_MAPPING_EXTENT_HPP

#include "Vector.hpp"

namespace photon_mapping {
    template <typename T>
    struct Extent2 {
        T x; T y;

        Extent2() {}

        Extent2(T x, T y)
            : x(x), y(y)
        {}

        template <typename OtherT>
        Extent2(Extent2<OtherT> const& other)
            : x(other.x), y(other.y)
        {}

        T const& operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }
    };

    typedef Extent2<int> Extent2i;
    typedef Extent2<float> Extent2f;

    template <typename T>
    Vector2<T> diagonal(Extent2<T> const& e) {
        return Vector2<T>(e.x, e.y);
    }

    template <typename T>
    struct Extent3 {
        T x; T y; T z;

        Extent3() {}

        Extent3(T x, T y, T z)
            : x(x), y(y), z(z)
        {}

        template <typename OtherT>
        Extent3(Extent3<OtherT> const& other)
            : x(other.x), y(other.y), z(other.z)
        {}

        T const& operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }
    };

    typedef Extent3<int> Extent3i;
    typedef Extent3<float> Extent3f;

    template <typename T>
    int maxAxis(Extent3<T> const& extent) {
        int maxAxis = -1;
        for (int axis = 0; axis < 3; ++axis) {
            if ((maxAxis == -1) || (extent[axis] > extent[maxAxis])) {
                maxAxis = axis;
            }
        }
        return maxAxis;
    }

    template <typename T>
    Vector3<T> diagonal(Extent3<T> const& e) {
        return Vector3<T>(e.x, e.y, e.z);
    }
}

#endif