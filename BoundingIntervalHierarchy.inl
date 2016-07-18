namespace photon_mapping {
    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::BoundingIntervalHierarchy(PrimitiveIterator primitivesBegin, PrimitiveIterator primitivesEnd) {
        rebuild(primitivesBegin, primitivesEnd);
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    void BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::rebuild(PrimitiveIterator primitivesBegin, PrimitiveIterator primitivesEnd) {
        m_bound = Segment3f::NOTHING;
        m_primitiveIterators.clear();
        for (auto primitive = primitivesBegin; primitive != primitivesEnd; ++primitive) {
            m_primitiveIterators.push_back(primitive);
            m_bound |= PrimitiveTraits::bound(*primitive);
        }
        m_root = std::unique_ptr<Node>(new Node(m_bound, m_primitiveIterators.begin(), m_primitiveIterators.end()));
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    inline Segment3f BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::bound() const {
        return m_bound;
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    inline bool BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::hit(Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const {
        float minDistance, maxDistance;
        if (!m_bound.hit(ray, minDistance, maxDistance)) {
            return false;
        }
        return m_root->hit(minDistance, maxDistance, ray, hitDistance, hit, hitPrimitiveIterator);
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::Node::Node(Segment3f const& bound, typename std::vector<PrimitiveIterator>::iterator primitiveIteratorsBegin, typename std::vector<PrimitiveIterator>::iterator primitiveIteratorsEnd) {
        m_axis = maxAxis(bound);
        float split = center(bound)[m_axis];
        auto belowPrimitiveIteratorsBegin = primitiveIteratorsBegin;
        auto belowPrimitiveIteratorsEnd = primitiveIteratorsBegin; 
        auto abovePrimitiveIteratorsBegin = primitiveIteratorsEnd;
        auto abovePrimitiveIteratorsEnd = primitiveIteratorsEnd;
        while (true) {
            while ((belowPrimitiveIteratorsEnd < abovePrimitiveIteratorsBegin) && (center(PrimitiveTraits::bound(**belowPrimitiveIteratorsEnd))[m_axis] <= split)) {
                ++belowPrimitiveIteratorsEnd;
            }
            while ((belowPrimitiveIteratorsEnd < abovePrimitiveIteratorsBegin) && (split < center(PrimitiveTraits::bound(**(abovePrimitiveIteratorsBegin - 1)))[m_axis])) {
                --abovePrimitiveIteratorsBegin;
            } 
            if (belowPrimitiveIteratorsEnd == abovePrimitiveIteratorsBegin) {
                break;
            }
            std::swap(*belowPrimitiveIteratorsEnd, *(abovePrimitiveIteratorsBegin - 1));
        }
        m_belowEdge = bound.min[m_axis];
        for (auto i = belowPrimitiveIteratorsBegin; i < belowPrimitiveIteratorsEnd; ++i) {
            m_belowEdge = max(m_belowEdge, PrimitiveTraits::bound(**i).max[m_axis]);
        }
        m_aboveEdge = bound.max[m_axis];
        for (auto i = abovePrimitiveIteratorsBegin; i < abovePrimitiveIteratorsEnd; ++i) {
            m_aboveEdge = min(m_aboveEdge, PrimitiveTraits::bound(**i).min[m_axis]);
        }

        if (((belowPrimitiveIteratorsEnd == primitiveIteratorsEnd) && (m_belowEdge == bound.max[m_axis])) || ((abovePrimitiveIteratorsBegin == primitiveIteratorsBegin) && (m_aboveEdge == bound.min[m_axis]))) {
            m_axis = -1;
            m_primitiveIteratorsBegin = primitiveIteratorsBegin;
            m_primitiveIteratorsEnd = primitiveIteratorsEnd;
            return;
        }

        if (belowPrimitiveIteratorsBegin != belowPrimitiveIteratorsEnd) {
            Segment3f belowBound = bound;
            belowBound.max[m_axis] = m_belowEdge;
            m_below = std::unique_ptr<Node>(new Node(belowBound, belowPrimitiveIteratorsBegin, belowPrimitiveIteratorsEnd));
        }

        if (abovePrimitiveIteratorsBegin != abovePrimitiveIteratorsEnd) {
            Segment3f aboveBound = bound;
            aboveBound.min[m_axis] = m_aboveEdge;
            m_above = std::unique_ptr<Node>(new Node(aboveBound, abovePrimitiveIteratorsBegin, abovePrimitiveIteratorsEnd));
        }
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    inline bool BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::Node::hit(float minDistance, float maxDistance, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const {
        if (m_axis == -1) {
            return hitLeaf(minDistance, maxDistance, ray, hitDistance, hit, hitPrimitiveIterator);
        } else {
            return hitFork(minDistance, maxDistance, ray, hitDistance, hit, hitPrimitiveIterator);
        }
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    inline bool BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::Node::hitFork(float minDistance, float maxDistance, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const {
        float belowEdgeDistance = (m_belowEdge - ray.origin[m_axis]) / ray.direction[m_axis];
        float aboveEdgeDistance = (m_aboveEdge - ray.origin[m_axis]) / ray.direction[m_axis];
        Node* lo = m_below.get();
        Node* hi = m_above.get();
        float loEdgeDistance = belowEdgeDistance;
        float hiEdgeDistance = aboveEdgeDistance;
        if (ray.direction[m_axis] < 0.0f) {
            std::swap(lo, hi);
            std::swap(loEdgeDistance, hiEdgeDistance);
        }
        if (loEdgeDistance <= hiEdgeDistance) {
            if (lo && (minDistance <= loEdgeDistance) && lo->hit(minDistance, min(loEdgeDistance, maxDistance), ray, hitDistance, hit, hitPrimitiveIterator)) {
                return true;
            }
            if (hi && (hiEdgeDistance <= maxDistance) && hi->hit(max(minDistance, hiEdgeDistance), maxDistance, ray, hitDistance, hit, hitPrimitiveIterator)) {
                return true;
            }
            return false;
        } else {
            bool hitHappened = false;
            float loHitDistance; typename PrimitiveTraits::Sample loHit; PrimitiveIterator loHitPrimitiveIterator;
            if (lo && (minDistance <= loEdgeDistance) && lo->hit(minDistance, min(loEdgeDistance, maxDistance), ray, loHitDistance, loHit, loHitPrimitiveIterator)) {
                if (!hitHappened || (loHitDistance < hitDistance)) {
                    hitDistance = loHitDistance;
                    hit = loHit;
                    hitPrimitiveIterator = loHitPrimitiveIterator;
                    hitHappened = true;
                }
            }
            float hiHitDistance; typename PrimitiveTraits::Sample hiHit; PrimitiveIterator hiHitPrimitiveIterator;
            if (hi && (hiEdgeDistance <= maxDistance) && hi->hit(max(minDistance, hiEdgeDistance), maxDistance, ray, hiHitDistance, hiHit, hiHitPrimitiveIterator)) {
                if (!hitHappened || (hiHitDistance < hitDistance)) {
                    hitDistance = hiHitDistance;
                    hit = hiHit;
                    hitPrimitiveIterator = hiHitPrimitiveIterator;
                    hitHappened = true;
                }
            }
            return hitHappened;
        }
    }

    template <typename Primitive, typename PrimitiveIterator, typename PrimitiveTraits>
    inline bool BoundingIntervalHierarchy<Primitive, PrimitiveIterator, PrimitiveTraits>::Node::hitLeaf(float, float, Ray const& ray, float& hitDistance, typename PrimitiveTraits::Sample& hit, PrimitiveIterator& hitPrimitiveIterator) const {
        bool triangleHitHappened = false;
        for (auto i = m_primitiveIteratorsBegin; i != m_primitiveIteratorsEnd; ++i) {
            float triangleHitDistance; typename PrimitiveTraits::Sample triangleHit; 
            if (PrimitiveTraits::hit(**i, ray, triangleHitDistance, triangleHit)) {
                if (!triangleHitHappened || (triangleHitDistance < hitDistance)) {
                    hitDistance = triangleHitDistance;
                    hit = triangleHit;
                    hitPrimitiveIterator = *i;
                    triangleHitHappened = true;
                }
            }
        }
        return triangleHitHappened;
    }
}