#include "Mesh.hpp"

namespace photon_mapping {
    static std::vector<Mesh::Triangle> const noTriangles;
    std::shared_ptr<Mesh const> const Mesh::DUMMY = std::make_shared<Mesh>(noTriangles.begin(), noTriangles.end());
}
