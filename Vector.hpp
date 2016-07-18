#ifndef PHOTON_MAPPING_VECTOR_HPP
#define PHOTON_MAPPING_VECTOR_HPP

#include <type_traits>

#include "mathFunctions.hpp"

namespace photon_mapping {
    template <typename T>
    struct Vector2 {
        T x; T y;

        Vector2() {}

        Vector2(T x, T y)
            : x(x), y(y)
        {}

        template <typename OtherT>
        Vector2(Vector2<OtherT> const& other)
            : x(other.x), y(other.y)
        {}

        T operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }

        static Vector2 const ZERO;
        static Vector2 const UNIT_X;
        static Vector2 const UNIT_Y;
    };

    template <typename T>
    Vector2<T> const Vector2<T>::ZERO = Vector2(T(0), T(0));
    template <typename T>
    Vector2<T> const Vector2<T>::UNIT_X = Vector2(T(1), T(0));
    template <typename T>
    Vector2<T> const Vector2<T>::UNIT_Y = Vector2(T(0), T(1));

    typedef Vector2<int> Vector2i;
    typedef Vector2<float> Vector2f;

    template <typename TV>
    inline Vector2<TV> operator+(Vector2<TV> const& v) {
        return Vector2<TV>(+v.x, +v.y);
    }

    template <typename TV>
    inline Vector2<TV> operator-(Vector2<TV> const& v) {
        return Vector2<TV>(-v.x, -v.y);
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator+(Vector2<TA> const& a, Vector2<TB> const& b) {
        return Vector2<typename std::common_type<TA, TB>::type>(
            a.x + b.x,  
            a.y + b.y
        );
    }

    template <typename TA, typename TB>
    inline Vector2<TA>& operator+=(Vector2<TA>& a, Vector2<TB> const& b) {
        a.x += b.x; 
        a.y += b.y;
        return a;
    }

    template <int n>
    inline Vector2f avg(Vector2f const (&v)[n], float const (&w)[n]) {
        Vector2f s = Vector2f::ZERO;
        for (int i = 0; i < n; ++i) {
            s += v[i] * w[i];
        }
        return s;
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator-(Vector2<TA> const& a, Vector2<TB> const& b) {
        return Vector2<typename std::common_type<TA, TB>::type>(
            a.x - b.x,  
            a.y - b.y
        );
    }

    template <typename TA, typename TB>
    inline Vector2<TA>& operator-=(Vector2<TA>& a, Vector2<TB> const& b) {
        a.x -= b.x; 
        a.y -= b.y;
        return a;
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator*(Vector2<TA> const& a, TB b) {
        return Vector2<typename std::common_type<TA, TB>::type>(
            a.x * b,  
            a.y * b
        );
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator*(TA a, Vector2<TB> const& b) {
        return Vector2<typename std::common_type<TA, TB>::type> (
            a * b.x,  
            a * b.y
        );
    }

    template <typename TA, typename TB>
    inline Vector2<TA>& operator*=(Vector2<TA>& a, TB b) {
        a.x *= b; 
        a.y *= b;
        return a;
    }

    template <typename TA, typename TB>
    inline Vector2<typename std::common_type<TA, TB>::type> operator/(Vector2<TA> const& a, TB b) {
        return Vector2<typename std::common_type<TA, TB>::type>(
            a.x / b,  
            a.y / b
        );
    }

    template <typename TA, typename TB>
    inline Vector2<TA>& operator/=(Vector2<TA>& a, TB b) {
        a.x /= b; 
        a.y /= b;
        return a;
    }

    template <typename TA, typename TB>
    inline typename std::common_type<TA, TB>::type wedge(Vector2<TA> const& a, Vector2<TB> const& b) {
        return a.x * b.y - a.y * b.x;
    }

    template <typename TA, typename TB>
    inline typename std::common_type<TA, TB>::type dot(Vector2<TA> const& a, Vector2<TB> const& b) {
        return a.x * b.x + a.y * b.y;
    }

    template <typename TV>
    inline TV sqrLength(Vector2<TV> const& v) {
        return dot(v, v);
    }

    inline float length(Vector2f const& v) {
        return sqrt(sqrLength(v));
    }

    inline Vector2f unit(Vector2f const& v) {
        return v / length(v);
    }

    inline Vector2f reflect(Vector2f const& i, Vector2f const& n) {
        return i - 2.0f * dot(i, n) * n;
    }

    inline Vector2f transmit(Vector2f const& i, Vector2f const& n, float k) {
        float ci = dot(i, n);
        float st2 = sqr(k) * (1.0f - sqr(ci));
        return k * i - (k * ci + sqrt(1.0f - st2)) * n;
    }

    template <typename T>
    struct Vector3 {
        T x; T y; T z;

        Vector3() {}

        Vector3(T x, T y, T z)
            : x(x), y(y), z(z)
        {}

        template <typename OtherT>
        Vector3(Vector3<OtherT> const& other)
            : x(other.x), y(other.y), z(other.z)
        {}

        template <typename OtherT>
        Vector3(Vector2<OtherT> const& v, T z)
            : x(v.x), y(v.y), z(z)
        {}

        T operator[](int i) const {
            return reinterpret_cast<T const*>(this)[i];
        }

        T& operator[](int i) {
            return reinterpret_cast<T*>(this)[i];
        }

        static Vector3 const ZERO;
        static Vector3 const UNIT_X;
        static Vector3 const UNIT_Y;
        static Vector3 const UNIT_Z;
    };

    template <typename T>
    Vector3<T> const Vector3<T>::ZERO = Vector3(T(0), T(0), T(0));
    template <typename T>
    Vector3<T> const Vector3<T>::UNIT_X = Vector3(T(1), T(0), T(0));
    template <typename T>
    Vector3<T> const Vector3<T>::UNIT_Y = Vector3(T(0), T(1), T(0));
    template <typename T>
    Vector3<T> const Vector3<T>::UNIT_Z = Vector3(T(0), T(0), T(1));

    typedef Vector3<int> Vector3i;
    typedef Vector3<float> Vector3f;

    template <typename TV>
    inline Vector3<TV> operator+(Vector3<TV> const& v) {
        return Vector3<TV>(+v.x, +v.y, +v.z);
    }

    template <typename TV>
    inline Vector3<TV> operator-(Vector3<TV> const& v) {
        return Vector3<TV>(-v.x, -v.y, -v.z);
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator+(Vector3<TA> const& a, Vector3<TB> const& b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.x + b.x,  
            a.y + b.y,
            a.z + b.z
        );
    }

    template <typename TA, typename TB>
    inline Vector3<TA>& operator+=(Vector3<TA>& a, Vector3<TB> const& b) {
        a.x += b.x; 
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    template <int n>
    inline Vector3f avg(Vector3f const (&v)[n], float const (&w)[n]) {
        Vector3f s = Vector3f::ZERO;
        for (int i = 0; i < n; ++i) {
            s += v[i] * w[i];
        }
        return s;
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator-(Vector3<TA> const& a, Vector3<TB> const& b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.x - b.x,  
            a.y - b.y,
            a.z - b.z
        );
    }

    template <typename TA, typename TB>
    inline Vector3<TA>& operator-=(Vector3<TA>& a, Vector3<TB> const& b) {
        a.x -= b.x; 
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator*(Vector3<TA> const& a, TB b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.x * b,  
            a.y * b,
            a.z * b
        );
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator*(TA a, Vector3<TB> const& b) {
        return Vector3<typename std::common_type<TA, TB>::type> (
            a * b.x,  
            a * b.y,
            a * b.z
        );
    }

    template <typename TA, typename TB>
    inline Vector3<TA>& operator*=(Vector3<TA>& a, TB b) {
        a.x *= b; 
        a.y *= b;
        a.z *= b;
        return a;
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> operator/(Vector3<TA> const& a, TB b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.x / b,  
            a.y / b,
            a.z / b
        );
    }

    template <typename TA, typename TB>
    inline Vector3<TA>& operator/=(Vector3<TA>& a, TB b) {
        a.x /= b; 
        a.y /= b;
        a.z /= b;
        return a;
    }

    template <typename TA, typename TB>
    inline Vector3<typename std::common_type<TA, TB>::type> cross(Vector3<TA> const& a, Vector3<TB> const& b) {
        return Vector3<typename std::common_type<TA, TB>::type>(
            a.y * b.z - a.z * b.y,  
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    template <typename TA, typename TB>
    inline typename std::common_type<TA, TB>::type dot(Vector3<TA> const& a, Vector3<TB> const& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template <typename TV>
    inline TV sqrLength(Vector3<TV> const& v) {
        return dot(v, v);
    }

    inline float length(Vector3f const& v) {
        return sqrt(sqrLength(v));
    }

    inline Vector3f unit(Vector3f const& v) {
        return v / length(v);
    }

    inline Vector3f reflect(Vector3f const& i, Vector3f const& n) {
        return i - 2.0f * dot(i, n) * n;
    }

    inline Vector3f transmit(Vector3f const& i, Vector3f const& n, float k) {
        float ci = dot(i, n);
        float st2 = sqr(k) * (1.0f - sqr(ci));
        return k * i - (k * ci + sqrt(1.0f - st2)) * n;
    }
}

#endif