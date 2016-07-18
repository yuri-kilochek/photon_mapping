#ifndef PHOTON_MAPPING_BOUNDING_INTERVAL_HIERARCHY_HPP
#define PHOTON_MAPPING_BOUNDING_INTERVAL_HIERARCHY_HPP

#include <memory>
#include <vector>

#include "Point.hpp"
#include "Vector.hpp"
#include "Segment.hpp"
#include "Ray.hpp"

namespace photon_mapping {
    template <typename Primitive>
    struct BoundingIntervalHierarchyPrimitiveTraits {
        typedef typename Primitive::Sample Sample;
        static Segment3f bound(Primitive const& primitive) {
            return primitive.bound();
        }
        static bool hit(Primitive const& primitive, Ray const& ray, float& hitDistance, Sample& hit) {
            return primitive.hit(ray, hitDistance, hit);
        }
    };

    template <typename Primitive>
    struct BoundingIntervalHierarchyPrimitiveTraits<std::shared_ptr<Primitive>> {
        typedef typename BoundingIntervalHierarchyPrimitiveTraits<Primitive>::Sample Sample;
        static Segment3f bound(std::shared_ptr<Primitive> const& primitive) {
            return BoundingIntervalHierarchyPrimitiveTraits<Primitive>::bound(*primitive);
        }
        static bool hit(std::shared_ptr<Primitive> const& primitive, Ray const& ray, float& hitDistance, Sample& hit) {
            return BoundingIntervalHierarchyPrimitiveTraits<Primitive>::hit(*primitive, ray, hitDistance, hit);
        }
    };

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits = BoundingIntervalHierarchyPrimitiveTraits<Primitive>>
    class BoundingIntervalHierarchy {
        public:
            BoundingIntervalHierarchy(PrimitiveIterator primitivesBegin, PrimitiveIterator primitivesEnd);

            void rebuild(PrimitiveIterator primitivesBegin, PrimitiveIterator primitivesEnd);

            Segment3f bound() const;
            bool hit(Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const;

        private:
            class Node {
                public:
                    Node(Segment3f const& bound, typename std::vector<PrimitiveIterator>::iterator primitiveIteratorsBegin, typename std::vector<PrimitiveIterator>::iterator primitiveIteratorsEnd);

                    bool hit(float minDistance, float maxDistance, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const;

                private:
                    int m_axis;
                    float m_belowEdge; float m_aboveEdge;
                    std::unique_ptr<Node> m_below; std::unique_ptr<Node> m_above;
                    typename std::vector<PrimitiveIterator>::const_iterator m_primitiveIteratorsBegin;
                    typename std::vector<PrimitiveIterator>::const_iterator m_primitiveIteratorsEnd;

                    bool hitLeaf(float minDistance, float maxDistance, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const;
                    bool hitFork(float minDistance, float maxDistance, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const;
            };

            Segment3f m_bound;
            std::vector<PrimitiveIterator> m_primitiveIterators;
            std::unique_ptr<Node> m_root;
    };
}

#include "BoundingIntervalHierarchy.inl"

#endif
