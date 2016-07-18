namespace photon_mapping {
    inline std::shared_ptr<Texture<TextureType::TrueColor> const> const& Material::emission() const {
        return m_emission;
    }

    inline float Material::emissionPower() const {
        return m_emissionPower;
    }

    inline std::shared_ptr<Texture<TextureType::TrueColor> const> const& Material::diffuse() const {
        return m_diffuse;
    }

    inline std::shared_ptr<Texture<TextureType::TrueColor> const> const& Material::specular() const {
        return m_specular;
    }

    inline float Material::specularPower() const {
        return m_specularPower;
    }

    inline std::shared_ptr<Texture<TextureType::NormalMap> const> const& Material::normal() const {
        return m_normal;
    }

    inline float Material::refractionIndex() const {
        return m_refractionIndex;
    }

    inline Color const& Material::transmission() const {
        return m_transmission;
    }
}