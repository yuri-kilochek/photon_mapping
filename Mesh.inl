#include <algorithm>

namespace photon_mapping {
    inline float Mesh::Triangle::area() const {
        return length(cross(edge01, edge02)) / 2.0;
    }
    
    inline Segment3f Mesh::Triangle::bound() const {
        return Segment3f(vertex) | Segment3f(vertex + edge01) | Segment3f(vertex + edge02);
    }

    inline Mesh::Triangle::Sample Mesh::Triangle::uniformOnSurface(Random& random) const {
        float vuw[3];
        vuw[1] = random.uniformInRange01();
        vuw[2] = random.uniformInRange01();
        vuw[0] = 1.0f - (vuw[1] + vuw[2]);
        if (vuw[0] < 0.0f) {
            vuw[0] = -vuw[0];
            vuw[1] = 1.0f - vuw[1];
            vuw[2] = 1.0f - vuw[2];
        }
        Sample sample;
        sample.position = vertex + edge01 * vuw[1] + edge02 * vuw[2];
        sample.tangent = unit(avg(tangents, vuw));
        sample.bitangent = unit(avg(bitangents, vuw));
        sample.normal = unit(avg(normals, vuw));
        sample.texcoord = avg(texcoords, vuw);
        sample.materialIndex = materialIndex;
        return sample;
    }

    inline bool Mesh::Triangle::hit(Ray const& ray, float& hitDistance, Sample& hit) const {
        float hitWuv[3];
        if (this->hit(ray, hitDistance, hitWuv)) {
            hit.position = ray.origin + ray.direction * hitDistance;
            hit.tangent = avg(tangents, hitWuv);
            hit.bitangent = avg(bitangents, hitWuv);
            hit.normal = avg(normals, hitWuv);
            hit.texcoord = avg(texcoords, hitWuv);
            hit.materialIndex = materialIndex;
            return true;
        }
        return false;
    }

    inline bool Mesh::Triangle::hit(Ray const& ray, float& hitDistance, float (&hitWuv)[3]) const {
        Vector3f k = ray.origin - vertex;
        Vector3f p = cross(ray.direction, edge02);
        Vector3f q = cross(k, edge01);
        float id = 1.0f / dot(edge01, p);
        hitDistance = dot(edge02, q) * id;
        hitWuv[1] = dot(k, p) * id;
        hitWuv[2] = dot(ray.direction, q) * id;
        hitWuv[0] = 1.0f - (hitWuv[1] + hitWuv[2]);
        return (0.0f < hitDistance) & (0.0f <= hitWuv[0]) & (0.0f <= hitWuv[1]) & (0.0f <= hitWuv[2]);
    }

    template <typename TriangleForwardIterator>
    Mesh::Mesh(TriangleForwardIterator trianglesBegin, TriangleForwardIterator trianglesEnd)
        : m_triangles(trianglesBegin, trianglesEnd)
        , m_inclusiveCumulativeTriangleProbabilities(0)
        , m_boundingIntervalHierarchy(m_triangles.begin(), m_triangles.end())
    {
        m_area = 0.0f;
        for (auto& triangle : m_triangles) {
            m_area += triangle.area();
            m_inclusiveCumulativeTriangleProbabilities.push_back(m_area); 
        }
        for (auto& inclusiveCumulativeTriangleProbability : m_inclusiveCumulativeTriangleProbabilities) {
            inclusiveCumulativeTriangleProbability /= m_area;
        }
    }

    inline float Mesh::area() const {
        return m_area;
    }

    inline Segment3f Mesh::bound() const {
        return m_boundingIntervalHierarchy.bound();
    }

    inline Mesh::Sample Mesh::uniformOnSurface(Random& random) const {
        int triangleIndex = std::lower_bound(m_inclusiveCumulativeTriangleProbabilities.begin(), m_inclusiveCumulativeTriangleProbabilities.end(), random.uniformInRange01()) - m_inclusiveCumulativeTriangleProbabilities.begin();
        auto triangle = m_triangles.begin() + triangleIndex;  
        Sample sample;
        static_cast<Triangle::Sample&>(sample) = triangle->uniformOnSurface(random);
        sample.triangle = triangle;
        return sample;
    }

    inline bool Mesh::hit(Ray const& ray, float& hitDistance, Sample& hit) const {
        return m_boundingIntervalHierarchy.hit(ray, hitDistance, hit, hit.triangle);
    }
}