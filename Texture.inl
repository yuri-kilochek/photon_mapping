#include <QColor>

#include "mathFunctions.hpp"

namespace photon_mapping {
    namespace internals {
        template <>
        struct TextureFormatDescriptor<TextureType::TrueColor> {
            typedef Color Element;
            static QImage formatImage(QImage const& image) {
                return image.convertToFormat(QImage::Format_RGB888);
            }
            static Color fetch(QImage const& image, Point2i const& point) {
                QColor c = image.pixel(point.x, point.y);
                return Color(c.red() / 255.0f, c.green() / 255.0f, c.blue() / 255.0f);
            }
            static QImage uniformImage(Color const& value) {
                QImage image(1, 1, QImage::Format_RGB888);
                image.fill(QColor(
                    clamp(0, int(value.r * 255), 255),
                    clamp(0, int(value.g * 255), 255),
                    clamp(0, int(value.b * 255), 255)    
                ));
                return image;
            }
            static QImage dummyImage() {
                return uniformImage(Color::BLACK);
            }
        };

        template <>
        struct TextureFormatDescriptor<TextureType::GrayScale> {
            typedef float Element;
            static QImage formatImage(QImage const& image) {
                QImage formattedImage(image.width(), image.height(), QImage::Format_Indexed8);
                formattedImage.setColorCount(256);
                for (int x = 0; x < image.width(); ++x) {
                    for (int y = 0; y < image.height(); ++y) {
                        formattedImage.setPixel(x, y, qGray(image.pixel(x, y)));
                    }
                }
                return formattedImage;
            }
            static float fetch(QImage const& image, Point2i const& point) {
                int c = image.pixelIndex(point.x, point.y);
                return c / 255.0f;
            }
            static QImage uniformImage(float value) {
                QImage image(1, 1, QImage::Format_Indexed8);
                image.setColorCount(256);
                image.setPixel(0, 0, qGray(
                    clamp(0, int(value * 255), 255),
                    clamp(0, int(value * 255), 255),
                    clamp(0, int(value * 255), 255)
                ));
                return image;
            }
            static QImage dummyImage() {
                return uniformImage(0.0f);
            }
        };

        template <>
        struct TextureFormatDescriptor<TextureType::NormalMap> {
            typedef Vector3f Element;
            static QImage formatImage(QImage const& image) {
                return image.convertToFormat(QImage::Format_RGB888);
            }
            static Vector3f fetch(QImage const& image, Point2i const& point) {
                QColor c = image.pixel(point.x, point.y);
                return Vector3f((c.red() / 255.0f - 0.5f) * 2.0f, (c.green() / 255.0f - 0.5f) * 2.0f, c.blue() / 255.0f);
            }
            static QImage uniformImage(Vector3f const& value) {
                QImage image(1, 1, QImage::Format_RGB888);
                image.fill(QColor((value.x / 2.0f + 0.5f) * 255, (value.y / 2.0f + 0.5f) * 255, value.z * 255));
                return image;
            }
            static QImage dummyImage() {
                return uniformImage(Vector3f::UNIT_Z);
            }
        };
    }

    template <TextureType type>
    std::shared_ptr<Texture<type> const> const Texture<type>::DUMMY = std::make_shared<Texture<type>>();

    template <TextureType type>
    Texture<type>::Texture()
        : m_filePath("")
        , m_image(internals::TextureFormatDescriptor<type>::dummyImage())
    {}

    template <TextureType type>
    Texture<type>::Texture(QString const& filePath)
        : m_filePath(filePath)
        , m_image(internals::TextureFormatDescriptor<type>::formatImage(QImage(filePath)))
    {}

    template <TextureType type>
    Texture<type>::Texture(typename internals::TextureFormatDescriptor<type>::Element const& value)
        : m_filePath("")
        , m_image(internals::TextureFormatDescriptor<type>::uniformImage(value))
    {}

    template <TextureType type>
    typename internals::TextureFormatDescriptor<type>::Element Texture<type>::fetch(Point2f const& texcoord) const {
        Point2i point = Point2i(
           (texcoord.x * (m_image.width() - 1)),
           ((1.0f - texcoord.y) * (m_image.height() - 1))
        );
        return internals::TextureFormatDescriptor<type>::fetch(m_image, point);
    }

    template <TextureType type>
    QString Texture<type>::filePath() const {
        return m_filePath;
    }
}