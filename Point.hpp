#ifndef PHOTON_MAPPING_POINT_HPP
#define PHOTON_MAPPING_POINT_HPP

#include <type_traits>

#include "mathFunctions.hpp"
#include "Vector.hpp"

namespace photon_mapping {
    template <typename T>
    struct Point2 {
        T x; T y;

        Point2() {}

        Point2(T x, T y)
            : x(x), y(y)
        {}

        template <typename OtherT>
        Point2(Point2<OtherT> const& other)
            : x(other.x), y(other.y)
        {}

        T operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }

        static Point2 const ORIGIN;
    };

    template <typename T>
    Point2<T> const Point2<T>::ORIGIN = Point2(T(0), T(0));

    typedef Point2<int> Point2i;
    typedef Point2<float> Point2f;

    template <typename TP, typename TV>
    inline Point2<typename std::common_type<TP, TV>::type> operator+(Point2<TP> const& p, Vector2<TV> const& v) {
        return Point2<typename std::common_type<TP, TV>::type>(
            p.x + v.x,  
            p.y + v.y
        );
    }

    template <typename TP, typename TV>
    inline Point2<TP>& operator+=(Point2<TP>& p, Vector2<TV> const& v) {
        p.x += v.x; 
        p.y += v.y;
        return p;
    }

    template <typename TP, typename TV>
    inline Point2<typename std::common_type<TP, TV>::type> operator-(Point2<TP> const& p, Vector2<TV> const& v) {
        return Point2<typename std::common_type<TP, TV>::type>(
            p.x - v.x,  
            p.y - v.y
        );
    }

    template <typename TP, typename TV>
    inline Point2<TP>& operator-=(Point2<TP>& p, Vector2<TV> const& v) {
        p.x -= v.x; 
        p.y -= v.y;
        return p;
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator-(Point2<TA> const& a, Point2<TB> const& b) {
        return Vector2<typename std::common_type<TA, TB>::type>(
            a.x - b.x,  
            a.y - b.y
        );
    }

    template <int n>
    inline Point2f avg(Point2f const (&p)[n], float const (&w)[n]) {
        Point2f s = Point2f::ORIGIN;
        for (int i = 0; i < n; ++i) {
            s.x += p[i].x * w[i];
            s.y += p[i].y * w[i];
        }
        return s;
    }

    template <typename TA, typename TB>
    inline typename std::common_type<TA, TB>::type sqrDistance(Point2<TA> const& a, Point2<TB> const& b) {
        return sqrLength(a - b);
    }

    template <typename TA>
    inline float distance(Point2<TA> const& a, Point2f const& b) {
        return sqrt(sqrDistance(a, b));
    }

    template <typename TB>
    inline float distance(Point2f const& a, Point2<TB> const& b) {
        return sqrt(sqrDistance(a, b));
    }

    inline float distance(Point2f const& a, Point2f const& b) {
        return sqrt(sqrDistance(a, b));
    }

    template <typename T>
    struct Point3 {
        T x; T y; T z;

        Point3() {}

        Point3(T x, T y, T z)
            : x(x), y(y), z(z)
        {}

        template <typename OtherT>
        Point3(Point3<OtherT> const& other)
            : x(other.x), y(other.y), z(other.z)
        {}

        template <typename OtherT>
        Point3(Point2<OtherT> const& v, T z)
            : x(v.x), y(v.y), z(z)
        {}

        T operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }

        static Point3 const ORIGIN;
    };

    template <typename T>
    Point3<T> const Point3<T>::ORIGIN = Point3(T(0), T(0), T(0));

    typedef Point3<int> Point3i;
    typedef Point3<float> Point3f;

    template <typename TP, typename TV>
    inline Point3<typename std::common_type<TP, TV>::type> operator+(Point3<TP> const& p, Vector3<TV> const& v) {
        return Point3<typename std::common_type<TP, TV>::type>(
            p.x + v.x,  
            p.y + v.y,
            p.z + v.z
        );
    }

    template <typename TP, typename TV>
    inline Point3<TP>& operator+=(Point3<TP>& p, Vector3<TV> const& v) {
        p.x += v.x; 
        p.y += v.y;
        p.z += v.z;
        return p;
    }

    template <typename TP, typename TV>
    inline Point3<typename std::common_type<TP, TV>::type> operator-(Point3<TP> const& p, Vector3<TV> const& v) {
        return Point3<typename std::common_type<TP, TV>::type>(
            p.x - v.x,  
            p.y - v.y,
            p.z - v.z
        );
    }

    template <typename TP, typename TV>
    inline Point3<TP>& operator-=(Point3<TP>& p, Vector3<TV> const& v) {
        p.x -= v.x; 
        p.y -= v.y;
        p.z -= v.z;
        return p;
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator-(Point3<TA> const& a, Point3<TB> const& b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.x - b.x,  
            a.y - b.y,
            a.z - b.z
        );
    }

    template <int n>
    inline Point3f avg(Point3f const (&p)[n], float const (&w)[n]) {
        Point3f s = Point3f::ORIGIN;
        for (int i = 0; i < n; ++i) {
            s.x += p[i].x * w[i];
            s.y += p[i].y * w[i];
            s.z += p[i].z * w[i];
        }
        return s;
    }

    template <typename TA, typename TB>
    inline typename std::common_type<TA, TB>::type sqrDistance(Point3<TA> const& a, Point3<TB> const& b) {
        return sqrLength(a - b);
    }

    template <typename TA>
    inline float distance(Point3<TA> const& a, Point3f const& b) {
        return sqrt(sqrDistance(a, b));
    }

    template <typename TB>
    inline float distance(Point3f const& a, Point3<TB> const& b) {
        return sqrt(sqrDistance(a, b));
    }

    inline float distance(Point3f const& a, Point3f const& b) {
        return sqrt(sqrDistance(a, b));
    }
}

#endif