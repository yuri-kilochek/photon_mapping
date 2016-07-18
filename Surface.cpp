#include "Surface.hpp"

#include <QScreen>

namespace photon_mapping {
    Surface::Surface()
        : m_extent(Extent2i(1, 1))
        , m_foreground()
        , m_background(m_extent.x, m_extent.y, QImage::Format_RGB32)
        , m_valid(true)
    {
        connect(&m_updateEmitter, &QTimer::timeout, [this]{
            m_guard.lock();
            if (!m_valid) {
                emit imageUpdated(m_foreground);
                m_foreground.detach();
                m_valid = true;
            }
            m_guard.unlock();
        });
        m_updateEmitter.start((1.0 / 60.0) * 1000);
    }

    void Surface::setExtent(Extent2i const& extent) {
        m_extent = extent;
        m_foreground = QImage(m_extent.x, m_extent.y, QImage::Format_RGB32);
        m_background = QImage(m_extent.x, m_extent.y, QImage::Format_RGB32);
    }

    void Surface::commitUpdate() {
        m_guard.lock();
        m_foreground = m_background;
        m_background.detach();
        m_valid = false;
        m_guard.unlock();
    }
}