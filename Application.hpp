#ifndef PHOTON_MAPPING_APPLICATION_HPP
#define PHOTON_MAPPING_APPLICATION_HPP

#include <memory>

#include <QApplication>
#include <QMainWindow>

#include "Scene.hpp"
#include "Camera.hpp"
#include "Surface.hpp"
#include "Renderer.hpp"

namespace photon_mapping {
    class Application
        : public QApplication
    {
        Q_OBJECT
        public:
            Application(int& argc, char** argv);

        private:
            std::shared_ptr<Object> pickedObject;
            Point3f pickedPoint;
    };
}

#endif