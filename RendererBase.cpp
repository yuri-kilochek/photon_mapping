#include "RendererBase.hpp"

#include <QMutexLocker>
#include <QDebug>

#include "mathFunctions.hpp"

namespace photon_mapping {
    RendererBase::RendererBase()
        : m_photons(10000)
        , m_photonsScattered(false)
        , m_mapLevelsBuilt(0)
        , m_photonMap(m_photons)
        , m_nearestPhotonBuffers(workerCount(), std::vector<PhotonMap::NearestPhoton>(100))
        , m_initialQuality(-4)
        , m_quality(m_initialQuality)
        , m_finalQuality(2)
        , m_maxRayBounceCount(4)
    {}

    void RendererBase::setScene(std::shared_ptr<Scene> const& scene) {
        m_scene = scene;
    }

    void RendererBase::invalidateScene() {
        m_scene->invalidate();
        m_photonsScattered = false;
        m_mapLevelsBuilt = 0;
        m_quality = m_initialQuality;
    }

    void RendererBase::setCamera(std::shared_ptr<Camera> const& camera) {
        m_camera = camera;
    }

    void RendererBase::invalidateCamera() {
         m_quality = m_initialQuality;
    }

    void RendererBase::setSurface(std::shared_ptr<Surface> const& surface) {
        m_surface = surface;
    }

    void RendererBase::setPhotonCount(int photonCount) {
        m_photons.resize(photonCount);
        m_photons.shrink_to_fit();
    }

    void RendererBase::setPhotonGatherCount(int photonGatherCount) {
        for (auto& nearestPhotonBuffer : m_nearestPhotonBuffers) {
            nearestPhotonBuffer.resize(photonGatherCount);
            nearestPhotonBuffer.shrink_to_fit();
        }
    }

    void RendererBase::setInitialQuality(int initialQuality) {
        m_initialQuality = initialQuality;
    }

    void RendererBase::setQuality(int quality) {
        m_quality = quality;
    }

    void RendererBase::setFinalQuality(int finalQuality) {
        m_finalQuality = finalQuality;
    }

    void RendererBase::setMaxRayBounceCount(int maxRayBounceCount) {
        m_maxRayBounceCount = maxRayBounceCount;
    }

    bool RendererBase::nextTask() {
        if (!m_photonsScattered) {
            return true;
        }
        if (m_mapLevelsBuilt < m_photonMap.levelCount()) {
            return true;
        }
        if (m_quality < m_finalQuality) {
            m_elapsedTimer.restart();
            return true;
        }
        return false;
    }

    void RendererBase::doTaskPart(int workerIndex) {
        if (!m_photonsScattered) {
            Random random(workerIndex + 1);
            int photonCountPerWorker = photonCount() / workerCount();
            int leftoverPhotonCount = photonCount() - photonCountPerWorker * workerCount();
            int thisWorkerPhotonsBegin = photonCountPerWorker * (workerIndex + 0) + min(workerIndex + 0, leftoverPhotonCount);
            int thisWorkerPhotonsEnd = photonCountPerWorker * (workerIndex + 1) + min(workerIndex + 1, leftoverPhotonCount);
            scatterPart(random, m_photons.begin() + thisWorkerPhotonsBegin, m_photons.begin() + thisWorkerPhotonsEnd);
            return;
        }
        if (m_mapLevelsBuilt < m_photonMap.levelCount()) {
            int subtreeCountPerWorker = m_photonMap.subtreeCountAtLevel(m_mapLevelsBuilt) / workerCount();
            int leftoverSubtreeCount = m_photonMap.subtreeCountAtLevel(m_mapLevelsBuilt) - subtreeCountPerWorker * workerCount();
            int thisWorkerSubtreesBegin = subtreeCountPerWorker * (workerIndex + 0) + min(workerIndex + 0, leftoverSubtreeCount);
            int thisWorkerSubtreesEnd = subtreeCountPerWorker * (workerIndex + 1) + min(workerIndex + 1, leftoverSubtreeCount);
            for (int subtree = thisWorkerSubtreesBegin; subtree < thisWorkerSubtreesEnd; ++subtree) {
                checkInterrupts();
                m_photonMap.buildLevelOnSubtree(m_mapLevelsBuilt, subtree);
            }
            return;
        }
        Extent2i sufraceExtent = m_surface->extent();
        int lineCount = sufraceExtent.y;
        int lineCountPerWorker = lineCount / workerCount();
        int leftoverLineCount = lineCount - lineCountPerWorker * workerCount();
        int thisWorkerLineBegin = lineCountPerWorker * (workerIndex + 0) + min(workerIndex + 0, leftoverLineCount);
        int thisWorkerLineEnd = lineCountPerWorker * (workerIndex + 1) + min(workerIndex + 1, leftoverLineCount);
        Segment2i segment = Segment2i(
            Point2i(0, thisWorkerLineBegin),
            Point2i(sufraceExtent.x, thisWorkerLineEnd)
        );
        renderPart(m_nearestPhotonBuffers[workerIndex], segment);
    }

    void RendererBase::taskDone() {
        if (!m_photonsScattered) {
            m_photonsScattered = true;
            return;
        }
        if (m_mapLevelsBuilt < m_photonMap.levelCount()) {
            ++m_mapLevelsBuilt;
            return;
        }
        if (m_quality < m_finalQuality) {
            if (m_quality == 0) {
                qDebug() << m_surface->extent().x << "x" << m_surface->extent().y << "=" << (m_surface->extent().x * m_surface->extent().y) << ":" << (m_elapsedTimer.nsecsElapsed() / 1e9);
            }
            m_surface->commitUpdate();
            ++m_quality;
            return;
        }
    }

    void RendererBase::taskAborted() {
        //m_elapsedTimer.restart();
    }
}