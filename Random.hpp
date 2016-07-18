#ifndef PHOTON_MAPPING_RANDOM_HPP
#define PHOTON_MAPPING_RANDOM_HPP

#include <cstdint>
#include <random>

#include "Vector.hpp"
#include "mathConstants.hpp"
#include "mathFunctions.hpp"

namespace photon_mapping {
    class Random {
        private:
            std::mt19937_64 mt;
            //std::uint32_t m_seed;

        public:
            Random(std::uint32_t seed)
                : mt(seed)
            {}

            std::uint64_t next() {
                return mt() - mt.min();
                //m_seed = (m_seed * 1103515245 + 12345) & 0x7FFFFFFF;
                //return m_seed;
            }
  
            float uniformInRange01() {
                return next() / float(mt.max() - mt.min());
                //return next() / float(0x7FFFFFFF);
            }

            float uniformInRange(float a, float b) {
                return a + (b - a) * uniformInRange01();
            }

            //Vector3f uniformDirection() {
            //    float p = uniformInRange(-pi(), +pi());
            //    float sp = sin(p);
            //    float cp = cos(p);
            //    float q = uniformInRange(-1.0f, +1.0f);
            //    float k = sqrt(1.0f - sqr(q));
            //    return Vector3f(
            //        k * cp,
            //        k * sp,
            //        q
            //    );
            //}

            Vector3f uniformDirection() {
                Vector3f d;
                float sl;
                do {
                    d.x = uniformInRange(-1.0f, 1.0f);
                    d.y = uniformInRange(-1.0f, 1.0f);
                    d.z = uniformInRange(-1.0f, 1.0f);
                    sl = sqrLength(d);
                } while (sl == 0.0f || sl > 1.0f);
                d /= sqrt(sl);
                return d;
            }

            Vector3f cosineDirection(Vector3f const& focusDirection) {
                return unit(focusDirection + uniformDirection());
            }

            Vector3f uniformSemidirection(Vector3f const& focusDirection) {
                Vector3f direction;
                do {
                    direction = uniformDirection();
                } while (dot(direction, focusDirection) < 0.0f);
                return direction;
            }
    };
}

#endif
