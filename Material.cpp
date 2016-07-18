#include "Material.hpp"

namespace photon_mapping {
    std::shared_ptr<Material const> const Material::DUMMY = std::make_shared<Material>();

    Material::Material()
        : m_emission(Texture<TextureType::TrueColor>::DUMMY)
        , m_emissionPower(1.0f)
        , m_diffuse(Texture<TextureType::TrueColor>::DUMMY)
        , m_specular(Texture<TextureType::TrueColor>::DUMMY)
        , m_specularPower(1.0f)
        , m_normal(Texture<TextureType::NormalMap>::DUMMY)
        , m_refractionIndex(1.0f)
        , m_transmission(Color::BLACK)
    {}

    void Material::setEmission(std::shared_ptr<Texture<TextureType::TrueColor> const> const& emission) {
        m_emission = emission;
    }

    void Material::setEmissionPower(float emissionPower) {
        m_emissionPower = emissionPower;
    }

    void Material::setDiffuse(std::shared_ptr<Texture<TextureType::TrueColor> const> const& diffuse) {
        m_diffuse = diffuse;
    }

    void Material::setSpecular(std::shared_ptr<Texture<TextureType::TrueColor> const> const& specular) {
        m_specular = specular;
    }

    void Material::setSpecularPower(float specularPower) {
        m_specularPower = specularPower;
    }

    void Material::setNormal(std::shared_ptr<Texture<TextureType::NormalMap> const> const& normal) {
        m_normal = normal;
    }

    void Material::setRefractionIndex(float refractionIndex) {
        m_refractionIndex = refractionIndex;
    }

    void Material::setTransmission(Color const& transmission) {
        m_transmission = transmission;
    }
}