namespace photon_mapping {
    inline Scene::Sample Scene::uniformOnSurface(Random& random) const {
        int objectIndex = std::lower_bound(m_inclusiveCumulativeObjectProbabilities.begin(), m_inclusiveCumulativeObjectProbabilities.end(), random.uniformInRange01()) - m_inclusiveCumulativeObjectProbabilities.begin();
        auto object = m_objects.begin() + objectIndex;  
        Sample sample;
        static_cast<Object::Sample&>(sample) = (*object)->uniformOnSurface(random);
        sample.object = object;
        return sample;
    }

    inline bool Scene::hit(Ray const& ray, float& hitDistance, Sample& hit) const {
        return m_boundingIntervalHierarchy.hit(ray, hitDistance, hit, hit.object);
    }
}