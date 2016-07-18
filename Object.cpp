#include "Object.hpp"

namespace photon_mapping {
    Object::Object()
        : m_transformation(Matrix43f::IDENTITY)
        , m_inverseTransformation(Matrix43f::IDENTITY)
        , m_mesh(Mesh::DUMMY)
        , m_materials()
    {}

    void Object::setName(QString const& name) {
        m_name = name;
    }

    void Object::setTransformation(Matrix43f const& transformation) {
        setTransformation(transformation, ~transformation);
    }
    void Object::setTransformation(Matrix43f const& transformation, Matrix43f const& inverseTransformation) {
        m_transformation = transformation;
        m_inverseTransformation = inverseTransformation;
    }

    void Object::setInverseTransformation(Matrix43f const& inverseTransformation) {
        setInverseTransformation(inverseTransformation, ~inverseTransformation);
    }
    void Object::setInverseTransformation(Matrix43f const& inverseTransformation, Matrix43f const& transformation) {
        m_inverseTransformation = inverseTransformation;
        m_transformation = transformation;
    }

    void Object::setMesh(std::shared_ptr<Mesh const> const& mesh) {
        m_mesh = mesh;
    }

    void Object::setMaterial(int index, std::shared_ptr<Material const> const& material) {
        if (index < 0 || m_materials.size() <= index) {
            m_materials.resize(index + 1, Material::DUMMY);
        }
        m_materials[index] = material;
        while (!m_materials.empty() && m_materials.back() == Material::DUMMY) {
            m_materials.pop_back();
        }
    }
}