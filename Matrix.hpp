#ifndef PHOTON_MAPPING_MATRIX_HPP
#define PHOTON_MAPPING_MATRIX_HPP

#include <type_traits>

#include "mathFunctions.hpp"
#include "Vector.hpp"
#include "Point.hpp"
#include "Extent.hpp"

namespace photon_mapping {
    template <typename T>
    struct Matrix43 {
        T s00; T s01; T s02;
        T s10; T s11; T s12;
        T s20; T s21; T s22;
        T s30; T s31; T s32;

        Matrix43() {}

        Matrix43(
            T s00, T s01, T s02,
            T s10, T s11, T s12,
            T s20, T s21, T s22,
            T s30, T s31, T s32
        )
            : s00(s00), s01(s01), s02(s02)
            , s10(s10), s11(s11), s12(s12)
            , s20(s20), s21(s21), s22(s22)
            , s30(s30), s31(s31), s32(s32)
        {}

        static Matrix43 const IDENTITY;

        static Matrix43 translation(Vector3<T> const& displacement) {
            return Matrix43(
                          T(1),           T(0),           T(0),
                          T(0),           T(1),           T(0),
                          T(0),           T(0),           T(1),
                displacement.x, displacement.y, displacement.z
            );
        }

        static Matrix43 rotation(Vector3<T> const& axis, T angle) {
            T s = sin(angle);
            T c = cos(angle); T k = T(1) - c;

            Vector3<T> ak = axis * k;
            Vector3<T> as = axis * s;

            return Matrix43(
                ak.x * axis.x +    c, ak.x * axis.y + as.z, ak.x * axis.z - as.y,
                ak.x * axis.y - as.z, ak.y * axis.y +    c, ak.y * axis.z + as.x,
                ak.x * axis.z + as.y, ak.y * axis.z - as.x, ak.z * axis.z +    c,
                                T(0),                 T(0),                 T(0)
            );
        }

        static Matrix43 rotation(Vector3<T> const& axis, T angle, Point3<T> const& point) {
            Vector3<T> displacement = point - Point3<T>::ORIGIN;
            return translation(-displacement) * rotation(axis, angle) * translation(+displacement);
        }

        static Matrix43 scaling(Extent3<T> const& factor) {
            return Matrix43(
                factor.x,     T(0),     T(0),
                    T(0), factor.y,     T(0),
                    T(0),     T(0), factor.z,
                    T(0),     T(0),     T(0)
            );
        }

        static Matrix43 scaling(Extent3<T> const& factor, Point3<T> const& point) {
            Vector3<T> displacement = point - Point3<T>::ORIGIN;
            return translation(-displacement) * scaling(factor) * translation(+displacement);
        }
    };

    template <typename T>
    Matrix43<T> const Matrix43<T>::IDENTITY = Matrix43(
        T(1), T(0), T(0),
        T(0), T(1), T(0),
        T(0), T(0), T(1),
        T(0), T(0), T(0)
    );

    typedef Matrix43<float> Matrix43f;

    template <typename T>
    inline Matrix43<T> operator~(Matrix43<T> const& m) {
        T d = m.s10 * m.s21 * m.s02 - m.s20 * m.s11 * m.s02
            + m.s20 * m.s01 * m.s12 - m.s00 * m.s21 * m.s12
            + m.s00 * m.s11 * m.s22 - m.s10 * m.s01 * m.s22;
        return Matrix43<T>(
            (m.s11 * m.s22 - m.s21 * m.s12) / d,
            (m.s21 * m.s02 - m.s01 * m.s22) / d,
            (m.s01 * m.s12 - m.s11 * m.s02) / d,

            (m.s20 * m.s12 - m.s10 * m.s22) / d,
            (m.s00 * m.s22 - m.s20 * m.s02) / d,
            (m.s10 * m.s02 - m.s00 * m.s12) / d,

            (m.s10 * m.s21 - m.s20 * m.s11) / d,
            (m.s20 * m.s01 - m.s00 * m.s21) / d,
            (m.s00 * m.s11 - m.s10 * m.s01) / d,

            (m.s30 * m.s21 * m.s12 - m.s20 * m.s31 * m.s12 - m.s30 * m.s11 * m.s22 + m.s10 * m.s31 * m.s22 + m.s20 * m.s11 * m.s32 - m.s10 * m.s21 * m.s32) / d,
            (m.s20 * m.s31 * m.s02 - m.s30 * m.s21 * m.s02 + m.s30 * m.s01 * m.s22 - m.s00 * m.s31 * m.s22 - m.s20 * m.s01 * m.s32 + m.s00 * m.s21 * m.s32) / d,
            (m.s30 * m.s11 * m.s02 - m.s10 * m.s31 * m.s02 - m.s30 * m.s01 * m.s12 + m.s00 * m.s31 * m.s12 + m.s10 * m.s01 * m.s32 - m.s00 * m.s11 * m.s32) / d
        );
    }
   
    template <typename TA, typename TB>
    inline Matrix43<typename std::common_type<TA, TB>::type> operator*(Matrix43<TA> const& a, Matrix43<TB> const& b) {
        return Matrix43<typename std::common_type<TA, TB>::type>(
            a.s00 * b.s00 + a.s01 * b.s10 + a.s02 * b.s20,
            a.s00 * b.s01 + a.s01 * b.s11 + a.s02 * b.s21,
            a.s00 * b.s02 + a.s01 * b.s12 + a.s02 * b.s22,

            a.s10 * b.s00 + a.s11 * b.s10 + a.s12 * b.s20,
            a.s10 * b.s01 + a.s11 * b.s11 + a.s12 * b.s21,
            a.s10 * b.s02 + a.s11 * b.s12 + a.s12 * b.s22,

            a.s20 * b.s00 + a.s21 * b.s10 + a.s22 * b.s20,
            a.s20 * b.s01 + a.s21 * b.s11 + a.s22 * b.s21,
            a.s20 * b.s02 + a.s21 * b.s12 + a.s22 * b.s22,

            a.s30 * b.s00 + a.s31 * b.s10 + a.s32 * b.s20 + b.s30,
            a.s30 * b.s01 + a.s31 * b.s11 + a.s32 * b.s21 + b.s31,
            a.s30 * b.s02 + a.s31 * b.s12 + a.s32 * b.s22 + b.s32
        );
    }

    template <typename TA, typename TB>
    inline Matrix43<TA>& operator*=(Matrix43<TA>& a, Matrix43<TB> const& b) {
        return a = a * b;
    }

    template <typename TV, typename TM>
    inline Vector3<typename std::common_type<TV, TM>::type> operator*(Vector3<TV> const& v, Matrix43<TM> const& m) {
        return Vector3<typename std::common_type<TV, TM>::type>(
            v.x * m.s00 + v.y * m.s10 + v.z * m.s20,
            v.x * m.s01 + v.y * m.s11 + v.z * m.s21,
            v.x * m.s02 + v.y * m.s12 + v.z * m.s22
        );
    }

    template <typename TV, typename TM>
    inline Vector3<TV>& operator*=(Vector3<TV>& v, Matrix43<TM> const& m) {
        return v = v * m;
    }

    template <typename TP, typename TM>
    inline Point3<typename std::common_type<TP, TM>::type> operator*(Point3<TP> const& p, Matrix43<TM> const& m) {
        return Point3<typename std::common_type<TP, TM>::type>(
            p.x * m.s00 + p.y * m.s10 + p.z * m.s20 + m.s30,
            p.x * m.s01 + p.y * m.s11 + p.z * m.s21 + m.s31,
            p.x * m.s02 + p.y * m.s12 + p.z * m.s22 + m.s32
        );
    }

    template <typename TV, typename TM>
    inline Point3<TV>& operator*=(Point3<TV>& p, Matrix43<TM> const& m) {
        return p = p * m;
    }
}

#endif