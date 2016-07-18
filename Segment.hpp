#ifndef PHOTON_MAPPING_SEGMENT_HPP
#define PHOTON_MAPPING_SEGMENT_HPP

#include "Point.hpp"
#include "Extent.hpp"
#include "Ray.hpp"
#include "Matrix.hpp"
#include "mathFunctions.hpp"

namespace photon_mapping {
    template <typename T>
    struct Segment2 {
        Point2<T> min; Point2<T> max;

        static Segment2 const NOTHING;
        static Segment2 const EVERYTHING;

        Segment2() {}

        Segment2(Point2<T> const& point)
            : min(point), max(point)
        {}

        Segment2(Point2<T> const& min, Point2<T> const& max)
            : min(min), max(max)
        {}

        template <typename OtherT>
        Segment2(Point2<OtherT> const& point)
            : min(point), max(point)
        {}

        template <typename OtherT>
        Segment2(Segment2<OtherT> const& that)
            : min(that.min), max(that.max)
        {}

        Point2<T> const& operator[](int i) const {
            return reinterpret_cast<Point2<T> const*>(this)[i];
        }

        Point2<T>& operator[](int i) {
            return reinterpret_cast<Point2<T>*>(this)[i];
        }


    };

    template <typename T>
    Segment2<T> const Segment2<T>::NOTHING = Segment2(
        Point2<T>(+inf(), +inf()),  
        Point2<T>(-inf(), -inf())
    );

    template <typename T>
    Segment2<T> const Segment2<T>::EVERYTHING = Segment2(
        Point2<T>(-inf(), -inf()),
        Point2<T>(+inf(), +inf())
    );

    typedef Segment2<int> Segment2i;
    typedef Segment2<float> Segment2f;

    template <typename T>
    Point2<T> center(Segment2<T> const& segment) {
        return Point2<T>(
            (segment.min.x + segment.max.x) / 2.0f,
            (segment.min.y + segment.max.y) / 2.0f
        );
    }

    template <typename T>
    Extent2<T> extent(Segment2<T> const& segment) {
        return Extent2<T>(
            segment.max.x - segment.min.x,
            segment.max.y - segment.min.y
        );
    }

    template <typename T>
    struct Segment3 {
        Point3<T> min; Point3<T> max;

        static Segment3 const NOTHING;
        static Segment3 const EVERYTHING;

        Segment3() {}

        Segment3(Point3<T> const& point)
            : min(point), max(point)
        {}

        Segment3(Point3<T> const& min, Point3<T> const& max)
            : min(min), max(max)
        {}

        template <typename OtherT>
        Segment3(Point3<OtherT> const& point)
            : min(point), max(point)
        {}

        template <typename OtherT>
        Segment3(Segment3<OtherT> const& that)
            : min(that.min), max(that.max)
        {}

        Point3<T> const& operator[](int i) const {
            return reinterpret_cast<Point3<T> const*>(this)[i];
        }

        Point3<T>& operator[](int i) {
            return reinterpret_cast<Point3<T>*>(this)[i];
        }

        bool hit(Ray const& ray, T& minDistance, T& maxDistance) const {
            minDistance = T(0);
            maxDistance = T(+inf());
            for (int axis = 0; axis < 2; ++axis) {
                T lo = (min[axis] - ray.origin[axis]) / ray.direction[axis];
                T hi = (max[axis] - ray.origin[axis]) / ray.direction[axis];
                minDistance = photon_mapping::max(photon_mapping::min(lo, hi), minDistance);
                maxDistance = photon_mapping::min(photon_mapping::max(lo, hi), maxDistance);
            }
            return minDistance <= maxDistance;
        }
    };

    template <typename T>
    Segment3<T> const Segment3<T>::NOTHING = Segment3(
        Point3<T>(+inf(), +inf(), +inf()),  
        Point3<T>(-inf(), -inf(), -inf())
    );

    template <typename T>
    Segment3<T> const Segment3<T>::EVERYTHING = Segment3(
        Point3<T>(-inf(), -inf(), -inf()),
        Point3<T>(+inf(), +inf(), +inf())
    );

    typedef Segment3<int> Segment3i;
    typedef Segment3<float> Segment3f;

    template <typename T>
    Point3<T> center(Segment3<T> const& segment) {
        return Point3<T>(
            (segment.min.x + segment.max.x) / 2.0f,
            (segment.min.y + segment.max.y) / 2.0f,
            (segment.min.z + segment.max.z) / 2.0f
        );
    }

    template <typename T>
    Point3<T> corner(Segment3<T> const& segment, int i) {
        Point3<T> corner;
        for (int a = 0; a < 3; ++a) {
            corner[a] = segment[(i >> a) & 1][a];
        }
        return corner;
    }

    template <typename T>
    Extent3<T> extent(Segment3<T> const& segment) {
        return Extent3<T>(
            segment.max.x - segment.min.x,
            segment.max.y - segment.min.y,
            segment.max.z - segment.min.z
        );
    }

    template <typename T>
    int maxAxis(Segment3<T> const& segment) {
        return maxAxis(extent(segment));
    }

    template <typename TA, typename TB>
    Segment3<typename std::common_type<TA, TB>::type> operator|(Segment3<TA> const& a, Segment3<TB> const& b) {
        return Segment3<typename std::common_type<TA, TB>::type>(
            Point3f(min(a.min.x, b.min.x), min(a.min.y, b.min.y), min(a.min.z, b.min.z)),
            Point3f(max(a.max.x, b.max.x), max(a.max.y, b.max.y), max(a.max.z, b.max.z))
        );
    }

    template <typename TA, typename TB>
    Segment3<TA>& operator|=(Segment3<TA>& a, Segment3<TB> const& b) {
        return a = a | b;
    }

    inline Segment3f operator*(Segment3f const& segment, Matrix43f const& matrix) {
        Segment3f transformedSegment = Segment3f::NOTHING;
        for (int i = 0; i < 8; ++i) {
            transformedSegment |= Segment3f(corner(segment, i) * matrix);
        }
        return transformedSegment;
    }

    inline Segment3f& operator*=(Segment3f& segment, Matrix43f const& matrix) {
        return segment = segment * matrix;
    }
}

#endif