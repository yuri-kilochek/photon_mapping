#ifndef PHOTON_MAPPING_SCENE_HPP
#define PHOTON_MAPPING_SCENE_HPP

#include <vector>
#include <memory>

#include "Object.hpp"

namespace photon_mapping {
    class Scene {
        public:
            struct Sample : Object::Sample {
                std::vector<std::shared_ptr<Object>>::const_iterator object;
            };

            Scene();

            bool empty() const;
            void addObject(std::shared_ptr<Object> const& object);
            void clear();

            void invalidate();

            Sample uniformOnSurface(Random& random) const;
            bool hit(Ray const& ray, float& hitDistance, Sample& hit) const;

        private:
            std::vector<std::shared_ptr<Object>> m_objects;
            std::vector<float> m_inclusiveCumulativeObjectProbabilities;
            BoundingIntervalHierarchy<std::shared_ptr<Object>, std::vector<std::shared_ptr<Object>>::const_iterator> m_boundingIntervalHierarchy;
    };
}

#include "Scene.inl"

#endif
