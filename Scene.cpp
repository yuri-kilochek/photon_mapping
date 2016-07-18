#include "Scene.hpp"

namespace photon_mapping {
    Scene::Scene()
        : m_boundingIntervalHierarchy(m_objects.begin(), m_objects.end())
    {}

    bool Scene::empty() const {
        return m_objects.empty();
    }

    void Scene::addObject(std::shared_ptr<Object> const& object) {
        m_objects.push_back(object);
    }

    void Scene::clear() {
        m_objects.clear();
    }

    void Scene::invalidate() {
        m_inclusiveCumulativeObjectProbabilities.clear();
        float area = 0.0f;
        for (auto& object : m_objects) {
            area += object->area();
            m_inclusiveCumulativeObjectProbabilities.push_back(area); 
        }
        for (auto& m_inclusiveCumulativeObjectProbability : m_inclusiveCumulativeObjectProbabilities) {
            m_inclusiveCumulativeObjectProbability /= area;
        }
        m_boundingIntervalHierarchy.rebuild(m_objects.begin(), m_objects.end());
    }
}