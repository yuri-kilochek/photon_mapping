namespace photon_mapping {
    inline std::shared_ptr<Scene> const& RendererBase::scene() const {
        return m_scene;
    }

    inline std::shared_ptr<Camera> const& RendererBase::camera() const {
        return m_camera;
    }

    inline std::shared_ptr<Surface> const& RendererBase::surface() const {
        return m_surface;
    }

    inline int RendererBase::photonCount() const {
        return m_photons.size();
    }

    inline int RendererBase::photonGatherCount() const {
        return m_nearestPhotonBuffers[0].size();
    }

    inline PhotonMap const& RendererBase::photonMap() const {
        return m_photonMap;
    }

    inline int RendererBase::initialQuality() const {
        return m_initialQuality;
    }

    inline int RendererBase::quality() const {
        return m_quality;
    }

    inline int RendererBase::finalQuality() const {
        return m_finalQuality;
    }

    inline int RendererBase::maxRayBounceCount() const {
        return m_maxRayBounceCount;
    }
}