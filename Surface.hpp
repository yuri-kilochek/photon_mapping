#ifndef PHOTON_MAPPING_SURFACE_HPP
#define PHOTON_MAPPING_SURFACE_HPP

#include <memory>

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QTimer>

#include "Extent.hpp"
#include "Color.hpp"
#include "Point.hpp"

namespace photon_mapping {
    class Surface
        : public QObject
    {
        Q_OBJECT
        public:
            Surface();

            Extent2i extent() const;
            void setExtent(Extent2i const& extent);

            void setPixel(Point2i const& position, Color const& color);
            void commitUpdate();

        signals:
            void imageUpdated(QImage const& image);

        private:
            Extent2i m_extent;
            QImage m_foreground;
            QImage m_background;
            bool m_valid;
            QMutex m_guard;
            QTimer m_updateEmitter;
    };
}

#include "Surface.inl"

#endif