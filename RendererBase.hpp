#ifndef PHOTON_MAPPING_RENDERER_BASE_HPP
#define PHOTON_MAPPING_RENDERER_BASE_HPP

#include <memory>
#include <vector>

#include <QElapsedTimer>

#include "WorkerSet.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Surface.hpp"
#include "Random.hpp"
#include "Photon.hpp"
#include "PhotonMap.hpp"

namespace photon_mapping {
    class RendererBase
        : public WorkerSet
    {
        Q_OBJECT
        public:
            RendererBase();
        
            std::shared_ptr<Scene> const& scene() const;
            void setScene(std::shared_ptr<Scene> const& scene);
            void invalidateScene();

            std::shared_ptr<Camera> const& camera() const;
            void setCamera(std::shared_ptr<Camera> const& camera);
            void invalidateCamera();

            std::shared_ptr<Surface> const& surface() const;
            void setSurface(std::shared_ptr<Surface> const& surface);

            int photonCount() const;
            void setPhotonCount(int photonCount);

            int photonGatherCount() const;
            void setPhotonGatherCount(int photonGatherCount);

            int initialQuality() const;
            void setInitialQuality(int initialQuality);

            int quality() const;
            void setQuality(int quality);

            int finalQuality() const;
            void setFinalQuality(int finalQuality);

            int maxRayBounceCount() const;
            void setMaxRayBounceCount(int maxRayBounceCount);

        protected:
            PhotonMap const& photonMap() const;

            virtual void scatterPart(Random& random, std::vector<Photon>::iterator begin, std::vector<Photon>::iterator end) = 0;
            virtual void renderPart(std::vector<PhotonMap::NearestPhoton>& nearestPhotons, Segment2i const& segment) = 0;

            virtual bool nextTask();
            virtual void doTaskPart(int workerIndex);
            virtual void taskDone();
            virtual void taskAborted();

        private:
            std::shared_ptr<Scene> m_scene;
            std::shared_ptr<Camera> m_camera;
            std::shared_ptr<Surface> m_surface;
            
            std::vector<Photon> m_photons;
            bool m_photonsScattered;
            int m_mapLevelsBuilt;
            PhotonMap m_photonMap;
            std::vector<std::vector<PhotonMap::NearestPhoton>> m_nearestPhotonBuffers;
            int m_initialQuality;
            int m_quality;
            int m_finalQuality;
            int m_maxRayBounceCount;
            QElapsedTimer m_elapsedTimer;
    };
}

#include "RendererBase.inl"

#endif
