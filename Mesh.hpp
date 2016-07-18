#ifndef PHOTON_MAPPING_MESH_HPP
#define PHOTON_MAPPING_MESH_HPP

#include <memory>
#include <vector>

#include "Point.hpp"
#include "Vector.hpp"
#include "Segment.hpp"
#include "Random.hpp"
#include "Ray.hpp"
#include "BoundingIntervalHierarchy.hpp"

namespace photon_mapping {
    class Mesh {
        public:
            class Triangle {
                public:
                    struct Sample {
                        Point3f position;
                        Vector3f tangent;
                        Vector3f bitangent;
                        Vector3f normal;
                        Point2f texcoord;
                        int materialIndex;
                    };

                    Point3f vertex; Vector3f edge01; Vector3f edge02;
                    Vector3f tangents[3];
                    Vector3f bitangents[3];
                    Vector3f normals[3];
                    Point2f texcoords[3];
                    int materialIndex;

                    float area() const;
                    Segment3f bound() const;
                    Sample uniformOnSurface(Random& random) const;
                    bool hit(Ray const& ray, float& hitDistance, Sample& hit) const;

                private:
                    bool hit(Ray const& ray, float& hitDistance, float (&hitWuv)[3]) const;
            };

            struct Sample : Triangle::Sample {
                std::vector<Triangle>::const_iterator triangle;
            };

            static std::shared_ptr<Mesh const> const DUMMY;

            template <typename TriangleForwardIterator>
            Mesh(TriangleForwardIterator trianglesBegin, TriangleForwardIterator trianglesEnd);
     
            float area() const;
            Segment3f bound() const;
            Sample uniformOnSurface(Random& random) const;
            bool hit(Ray const& ray, float& hitDistance, Sample& hit) const;

        private:
            std::vector<Triangle> m_triangles;
            float m_area;
            std::vector<float> m_inclusiveCumulativeTriangleProbabilities;
            BoundingIntervalHierarchy<Triangle, std::vector<Triangle>::const_iterator> m_boundingIntervalHierarchy;
    };
}

#include "Mesh.inl"

#endif
