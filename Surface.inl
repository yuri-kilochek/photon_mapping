namespace photon_mapping {
    inline Extent2i Surface::extent() const {
        return m_extent;
    }

    inline void Surface::setPixel(Point2i const& position, Color const& color) {
        m_background.setPixel(QPoint(position.x, m_extent.y - 1 - position.y), qRgb(
            clamp(0, int(color.r * 255), 255),
            clamp(0, int(color.g * 255), 255),
            clamp(0, int(color.b * 255), 255)
        ));
    }
}