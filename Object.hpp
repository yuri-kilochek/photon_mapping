#ifndef PHOTON_MAPPING_OBJECT_HPP
#define PHOTON_MAPPING_OBJECT_HPP

#include <memory>

#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Color.hpp"
#include "Vector.hpp"
#include "Ray.hpp"

namespace photon_mapping {
    class Object {
        public:
            struct Sample {
                Point3f position;
                Color emission;
                float emissionPower;
                Color diffuse;
                Color specular;
                float specularPower;
                Vector3f normal;
                float refractionIndex;
                Color transmission;
                std::vector<Mesh::Triangle>::const_iterator triangle;
            };

            Object();

            QString const& name() const;
            void setName(QString const& name);

            Matrix43f const& transformation() const;
            void setTransformation(Matrix43f const& transformation);
            void setTransformation(Matrix43f const& transformation, Matrix43f const& inverseTransformation);

            Matrix43f const& inverseTransformation() const;
            void setInverseTransformation(Matrix43f const& inverseTransformation);
            void setInverseTransformation(Matrix43f const& inverseTransformation, Matrix43f const& transformation);

            std::shared_ptr<Mesh const> const& mesh() const;
            void setMesh(std::shared_ptr<Mesh const> const& mesh);

            std::shared_ptr<Material const> const& material(int index) const;
            void setMaterial(int index, std::shared_ptr<Material const> const& material);

            float area() const;
            Segment3f bound() const;
            Sample uniformOnSurface(Random& random) const;
            bool hit(Ray const& ray, float& hitDistance, Sample& hit) const;

        private:    
            QString m_name;

            Matrix43f m_transformation;
            Matrix43f m_inverseTransformation;

            std::shared_ptr<Mesh const> m_mesh;
            std::vector<std::shared_ptr<Material const>> m_materials;
    };
}

#include "Object.inl"

#endif