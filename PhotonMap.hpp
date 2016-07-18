#ifndef PHOTON_MAPPING_PHOTON_MAP_HPP
#define PHOTON_MAPPING_PHOTON_MAP_HPP

#include <vector>
#include <algorithm>

#include "Photon.hpp"
#include "Segment.hpp"

#include <QDebug>

namespace photon_mapping {
    class PhotonMap {
        public:
            PhotonMap(std::vector<Photon>& photons)
                : m_photonsBound()
                , m_photons(photons)
            {}

            int levelCount() const {
                return ceiling(log(m_photons.size() + 1));
            }

            static int subtreeCountAtLevel(int level) {
                return 1 << level;
            }

            void buildLevelOnSubtree(int level, int subtree) {
                if (level == 0) {
                    m_photonsBound = Segment3f::NOTHING;
                    for (auto& photon : m_photons) {
                        m_photonsBound |= Segment3f(photon.position);
                    }
                }
                buildLevelOnSubtree(level, subtree, m_photons.begin(), m_photons.end(), m_photonsBound);
            }

            struct NearestPhoton {
                std::vector<Photon>::const_iterator photon;
                float sqrDistance;
            };

            std::vector<NearestPhoton>::iterator nearest(Point3f const& position, std::vector<NearestPhoton>::iterator nearestBegin, std::vector<NearestPhoton>::iterator nearestEnd) const {
                return nearest(m_photons.begin(), m_photons.end(), m_photonsBound, position, nearestBegin, nearestBegin, nearestEnd);
            }

        private:
            Segment3f m_photonsBound;
            std::vector<Photon>& m_photons;

            void buildLevelOnSubtree(int level, int subtree, std::vector<Photon>::iterator begin, std::vector<Photon>::iterator end, Segment3f const& bound) {
                if (begin == end) {
                    return;
                }
                int splitAxis = maxAxis(bound);
                if (level == 0) {
                    std::sort(begin, end, [=](Photon const& a, Photon const& b) {
                        return a.position[splitAxis] < b.position[splitAxis];
                    });
                    return;
                }
                std::vector<Photon>::iterator median = begin + (end - begin) / 2;
                int mask = 1 << (level - 1);
                if ((subtree & mask) == 0) {
                    Segment3f belowBound = bound;
                    belowBound.max[splitAxis] = median->position[splitAxis];
                    buildLevelOnSubtree(level - 1, subtree & ~mask, begin, median, belowBound);
                } else {
                    Segment3f aboveBound = bound;
                    aboveBound.min[splitAxis] = median->position[splitAxis];
                    buildLevelOnSubtree(level - 1, subtree & ~mask, median + 1, end, aboveBound);
                }
            }

            struct SqrDistanceLess {
                bool operator()(NearestPhoton const& a, NearestPhoton const& b) const {
                    return a.sqrDistance < b.sqrDistance;
                }
            };

            static std::vector<NearestPhoton>::iterator nearest(std::vector<Photon>::const_iterator begin, std::vector<Photon>::const_iterator end, Segment3f const& bound, Point3f const& position, std::vector<NearestPhoton>::iterator nearestBegin, std::vector<NearestPhoton>::iterator nearestNext, std::vector<NearestPhoton>::iterator nearestEnd) {
                if (begin == end) {
                    return nearestNext;
                }
                
                std::vector<Photon>::const_iterator median = begin + (end - begin) / 2;
                float medianSqrDistance = sqrDistance(median->position, position);

                if (nearestNext == nearestEnd) {
                    if (medianSqrDistance < nearestBegin->sqrDistance) {
                        std::pop_heap(nearestBegin, nearestNext--, SqrDistanceLess());
                        nearestNext->photon = median;
                        nearestNext->sqrDistance = medianSqrDistance;
                        std::push_heap(nearestBegin, ++nearestNext, SqrDistanceLess());
                    }
                } else {
                    nearestNext->photon = median;
                    nearestNext->sqrDistance = medianSqrDistance;
                    std::push_heap(nearestBegin, ++nearestNext, SqrDistanceLess());
                }

                int splitAxis = maxAxis(bound);
                if (position[splitAxis] <= median->position[splitAxis]) {
                    Segment3f belowBound = bound;
                    belowBound.max[splitAxis] = median->position[splitAxis];
                    nearestNext = nearest(begin, median, belowBound, position, nearestBegin, nearestNext, nearestEnd);
                } else {
                    Segment3f aboveBound = bound;
                    aboveBound.min[splitAxis] = median->position[splitAxis];
                    nearestNext = nearest(median + 1, end, aboveBound, position, nearestBegin, nearestNext, nearestEnd);
                }

                if (nearestNext != nearestEnd || sqr(median->position[splitAxis] - position[splitAxis]) < nearestBegin->sqrDistance) {
                    if (position[splitAxis] <= median->position[splitAxis]) {
                        Segment3f aboveBound = bound;
                        aboveBound.min[splitAxis] = median->position[splitAxis];
                        nearestNext = nearest(median + 1, end, aboveBound, position, nearestBegin, nearestNext, nearestEnd);
                    } else {
                        Segment3f belowBound = bound;
                        belowBound.max[splitAxis] = median->position[splitAxis];
                        nearestNext = nearest(begin, median, belowBound, position, nearestBegin, nearestNext, nearestEnd);
                    }
                }
                return nearestNext;
            }
    };
}

#endif
