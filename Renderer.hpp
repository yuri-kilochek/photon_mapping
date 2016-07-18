#ifndef PHOTON_MAPPING_RENDERER_HPP
#define PHOTON_MAPPING_RENDERER_HPP

#include <memory>

#include "RendererBase.hpp"

namespace photon_mapping {
    class Renderer
        : public RendererBase
    {
        Q_OBJECT
        public:

        private:
            Photon Renderer::emitAndScatter(Random &random);
            virtual void scatterPart(Random& random, std::vector<Photon>::iterator begin, std::vector<Photon>::iterator end);
            Color render(std::vector<PhotonMap::NearestPhoton>& nearestPhotons, Ray const& ray, int rayBounceCount = 0) const;
            virtual void renderPart(std::vector<PhotonMap::NearestPhoton>& nearestPhotons, Segment2i const& segment);
    };
}

#include "Renderer.inl"

#endif
