namespace photon_mapping {
    inline QString const& Object::name() const {
        return m_name;
    }

    inline Matrix43f const& Object::transformation() const {
        return m_transformation;
    }

    inline Matrix43f const& Object::inverseTransformation() const {
        return m_inverseTransformation;
    }

    inline std::shared_ptr<Mesh const> const& Object::mesh() const {
        return m_mesh;
    }

    inline std::shared_ptr<Material const> const& Object::material(int index) const {
        if (0 <= index && index < m_materials.size()) {
            return m_materials[index];
        }
        return Material::DUMMY;
    }

    inline float Object::area() const {
        //FIXME: adjust according to transformation
        return mesh()->area();
    }

    inline Segment3f Object::bound() const {
        return mesh()->bound() * transformation();
    }

    inline Object::Sample Object::uniformOnSurface(Random& random) const {
        Mesh::Sample meshSample = mesh()->uniformOnSurface(random);
        Sample sample;
        sample.position = meshSample.position * transformation();
        sample.emission = material(meshSample.materialIndex)->emission()->fetch(meshSample.texcoord); 
        sample.emissionPower = material(meshSample.materialIndex)->emissionPower();
        sample.diffuse = material(meshSample.materialIndex)->diffuse()->fetch(meshSample.texcoord);
        sample.specular = material(meshSample.materialIndex)->specular()->fetch(meshSample.texcoord);
        sample.specularPower = material(meshSample.materialIndex)->specularPower();
        sample.normal = material(meshSample.materialIndex)->normal()->fetch(meshSample.texcoord) * ~Matrix43f(
            meshSample.tangent.x, meshSample.bitangent.x, meshSample.normal.x,
            meshSample.tangent.y, meshSample.bitangent.y, meshSample.normal.y,
            meshSample.tangent.z, meshSample.bitangent.z, meshSample.normal.z,
                            0.0f,                0.0f,             0.0f
        ) * transformation();
        sample.refractionIndex = material(meshSample.materialIndex)->refractionIndex();
        sample.transmission = material(meshSample.materialIndex)->transmission();
        sample.triangle = meshSample.triangle;
        return sample;
    }

    inline bool Object::hit(Ray const& ray, float& hitDistance, Sample& hit) const{
        float meshHitDistance; Mesh::Sample meshHit;
        if (mesh()->hit(ray * inverseTransformation(), meshHitDistance, meshHit)) {
            hit.position = meshHit.position * transformation();
            hit.emission = material(meshHit.materialIndex)->emission()->fetch(meshHit.texcoord);
            hit.emissionPower = material(meshHit.materialIndex)->emissionPower();
            hit.diffuse = material(meshHit.materialIndex)->diffuse()->fetch(meshHit.texcoord);
            hit.specular = material(meshHit.materialIndex)->specular()->fetch(meshHit.texcoord);
            hit.specularPower = material(meshHit.materialIndex)->specularPower();
            hit.normal = material(meshHit.materialIndex)->normal()->fetch(meshHit.texcoord) * ~Matrix43f(
                meshHit.tangent.x, meshHit.bitangent.x, meshHit.normal.x,
                meshHit.tangent.y, meshHit.bitangent.y, meshHit.normal.y,
                meshHit.tangent.z, meshHit.bitangent.z, meshHit.normal.z,
                             0.0f,                0.0f,             0.0f
            ) * transformation();
            hit.refractionIndex = material(meshHit.materialIndex)->refractionIndex();
            hit.transmission = material(meshHit.materialIndex)->transmission();
            hit.triangle = meshHit.triangle;
            hitDistance = distance(hit.position, ray.origin);
            return true;
        }
        return false;
    }
}