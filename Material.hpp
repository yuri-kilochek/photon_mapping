#ifndef PHOTON_MAPPING_MATERIAL_HPP
#define PHOTON_MAPPING_MATERIAL_HPP

#include <memory>

#include "Color.hpp"
#include "Texture.hpp"

namespace photon_mapping {
    class Material {
        public:
            static std::shared_ptr<Material const> const DUMMY;

            Material();

            std::shared_ptr<Texture<TextureType::TrueColor> const> const& emission() const;
            void setEmission(std::shared_ptr<Texture<TextureType::TrueColor> const> const& emission);

            float emissionPower() const;
            void setEmissionPower(float emissionPower);

            std::shared_ptr<Texture<TextureType::TrueColor> const> const& diffuse() const;
            void setDiffuse(std::shared_ptr<Texture<TextureType::TrueColor> const> const& diffuse);

            std::shared_ptr<Texture<TextureType::TrueColor> const> const& specular() const;
            void setSpecular(std::shared_ptr<Texture<TextureType::TrueColor> const> const& specular);

            float specularPower() const;
            void setSpecularPower(float specularPower);

            std::shared_ptr<Texture<TextureType::NormalMap> const> const& normal() const;
            void setNormal(std::shared_ptr<Texture<TextureType::NormalMap> const> const& normal);

            float refractionIndex() const;
            void setRefractionIndex(float refractionIndex);

            Color const& transmission() const;
            void setTransmission(Color const& transmission);

        private:    
            std::shared_ptr<Texture<TextureType::TrueColor> const> m_emission;
            float m_emissionPower;
            std::shared_ptr<Texture<TextureType::TrueColor> const> m_diffuse;
            std::shared_ptr<Texture<TextureType::TrueColor> const> m_specular;
            float m_specularPower;
            std::shared_ptr<Texture<TextureType::NormalMap> const> m_normal;
            float m_refractionIndex;
            Color m_transmission;
    };
}

#include "Material.inl"

#endif