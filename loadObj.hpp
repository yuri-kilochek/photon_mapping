#ifndef PHOTON_MAPPING_LOAD_OBJ_HPP
#define PHOTON_MAPPING_LOAD_OBJ_HPP

#include <memory>

#include <QString>

#include "Scene.hpp"

namespace photon_mapping {
    void loadObj(std::shared_ptr<Scene> const& scene, QString const& objFilePath);
}

#endif