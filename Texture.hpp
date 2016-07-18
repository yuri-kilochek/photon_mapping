#ifndef PHOTON_MAPPING_TEXTURE_HPP
#define PHOTON_MAPPING_TEXTURE_HPP

#include <memory>

#include <QImage>

#include "Color.hpp"
#include "Point.hpp"

namespace photon_mapping {
    enum class TextureType {
        TrueColor,
        GrayScale,
        NormalMap, 
    };

    namespace internals {
        template <TextureType type>
        struct TextureFormatDescriptor;
    }

    template <TextureType type>
    class Texture {
        public:
            static std::shared_ptr<Texture const> const DUMMY;

            Texture();
            Texture(QString const& filePath);
            Texture(typename internals::TextureFormatDescriptor<type>::Element const& value);

            typename internals::TextureFormatDescriptor<type>::Element fetch(Point2f const& texcoord) const;

            QString filePath() const;

        private:
            QString m_filePath;
            QImage m_image;
    };
}

#include "Texture.inl"

#endif