#include "Application.hpp"

#include <QMainWindow>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDockWidget>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QSpinBox>
#include <QStatusBar>

#include "Scene.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Label.hpp"

#include "loadObj.hpp"

#include <QDebug>

namespace photon_mapping {
    Application::Application(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        QStringList paths = libraryPaths();
        paths.append(".");
        paths.append("imageformats");
        paths.append("platforms");
        setLibraryPaths(paths);

        auto scene = std::make_shared<Scene>();
        //loadObj(scene, "models/cornell-box/CornellBox-Sphere.obj");
        //scene->invalidate();
        
        auto camera = std::make_shared<Camera>();
        camera->setFieldOfView(degToRad(60.0f));
        camera->setTransformation(
            camera->transformation()
            * Matrix43f::translation(Vector3f(0.0f, 1.0f, 3.0f))
        );

        auto surface = std::make_shared<Surface>();

        auto renderer = std::make_shared<Renderer>();
        renderer->setScene(scene);
        renderer->setCamera(camera);
        renderer->setSurface(surface);
        renderer->start();
        
        //FrequencyMeter* frameFrequencyMeter = new FrequencyMeter(this);
        //frameFrequencyMeter->setSampleCount(10);
        //frameFrequencyMeter->setFrequency((1.0 / 60.0) * 1000);
        //connect(frameUpdater, &QTimer::timeout, frameFrequencyMeter, &FrequencyMeter::operator());

        QMainWindow* mainWindow = new QMainWindow();
        mainWindow->setMenuBar([&]{
            QMenuBar* menuBar = new QMenuBar(mainWindow);
            menuBar->addMenu([&]{
                QMenu* fileMenu = new QMenu();
                fileMenu->setTitle("File");
                fileMenu->addAction([&]{
                    QAction* fileOpenAction = new QAction(fileMenu);
                    fileOpenAction->setText("Open...");
                    connect(fileOpenAction, &QAction::triggered, [=](bool) {
                        QString objFilePath = QFileDialog::getOpenFileName(mainWindow, "Open .OBJ file", "models/", "OBJ File (*.obj)");
                        if (!objFilePath.isNull()) {
                            renderer->abort();
                            scene->clear();
                            loadObj(scene, objFilePath);
                            renderer->invalidateScene();
                            renderer->start();
                        }
                    });
                    return fileOpenAction;
                }());
                return fileMenu;
            }());
            return menuBar;
        }());
        mainWindow->setCentralWidget([&]{
            Label* viewLabel = new Label(mainWindow);
            viewLabel->setFocusPolicy(Qt::StrongFocus);
            viewLabel->setFocus(Qt::ActiveWindowFocusReason);
            connect(surface.get(), &Surface::imageUpdated, [=](QImage const& image) {
                viewLabel->setPixmap(QPixmap::fromImage(image));
            });
            connect(viewLabel, &Label::keyPressed, [=](Qt::Key key){
                float cameraVelocity = 0.1f;
                switch (key) {
                    case Qt::Key_W:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(camera->forward() * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                    case Qt::Key_S:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(camera->back() * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                    case Qt::Key_A:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(camera->left() * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                    case Qt::Key_D:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(camera->right() * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                    case Qt::Key_Space:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(+Vector3f::UNIT_Y * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                    case Qt::Key_Control:
                        renderer->abort();
                        camera->setTransformation(camera->transformation() * Matrix43f::translation(-Vector3f::UNIT_Y * cameraVelocity));
                        renderer->invalidateCamera();
                        renderer->start();
                        break;
                }
            });
            connect(viewLabel, &Label::resized, [=](Extent2i const& extent) {
                if (extent.x != 0 && extent.y != 0) {
                    renderer->abort();
                    surface->setExtent(extent);
                    renderer->invalidateCamera();
                    renderer->start();
                }
            });
            auto pickRay = [=] {
                Extent2i surfaceExtent = surface->extent();
                Point2i mousePosition = *viewLabel->mousePosition();
                mousePosition.y = surfaceExtent.y - 1 - mousePosition.y;
                return camera->cast(mousePosition, surfaceExtent);
            };
            auto pickObject = [=]() -> std::shared_ptr<Object> {
                float pickDistance; Scene::Sample pick;
                if (scene->hit(pickRay(), pickDistance, pick)) {
                    return *pick.object;
                }
                return nullptr;
            };
            auto pickPoint = [=]() -> Point3f {
                float pickDistance; Scene::Sample pick;
                if (scene->hit(pickRay(), pickDistance, pick)) {
                    return pick.position;
                }
                return Point3f();
            };
            connect(viewLabel, &Label::mouseButtonPressed, [=](Qt::MouseButton mousebutton) {
                if (mousebutton == Qt::RightButton) {
                    setOverrideCursor(Qt::BlankCursor);
                }
                if (mousebutton == Qt::LeftButton) {
                    pickedObject = pickObject();
                    if (pickedObject) {
                        pickedPoint = pickPoint();
                    }
                }
            });
            connect(viewLabel, &Label::mouseMoved, [=](Point2i const& position, Vector2i const& displacement) mutable {
                if (!viewLabel->isMouseButtonPressed(Qt::LeftButton) && viewLabel->isMouseButtonPressed(Qt::RightButton)) {
                    static float const cameraAngularVelocity = degToRad(0.2f);
                    renderer->abort();
                    camera->setTransformation(
                        camera->transformation()
                        * Matrix43f::rotation(-Vector3f::UNIT_Y, displacement.x * cameraAngularVelocity, camera->position())
                        * Matrix43f::rotation(camera->right(), -displacement.y * cameraAngularVelocity, camera->position())
                    );
                    renderer->invalidateCamera();
                    renderer->start();
                }
                if (viewLabel->isMouseButtonPressed(Qt::LeftButton) && !viewLabel->isMouseButtonPressed(Qt::RightButton)) {
                    Ray ray = pickRay();
                    Point3f newPickedPoint = ray.origin + ray.direction * distance(camera->position(), pickedPoint);
                    renderer->abort();
                    pickedObject->setTransformation(pickedObject->transformation() * Matrix43f::translation(newPickedPoint - pickedPoint));
                    renderer->invalidateScene();
                    renderer->start();
                    pickedPoint = newPickedPoint;
                }
                if (viewLabel->isMouseButtonPressed(Qt::LeftButton) && viewLabel->isMouseButtonPressed(Qt::RightButton)) {
                    static float const objectAngularVelocity = degToRad(0.3f);
                    renderer->abort();
                    pickedObject->setTransformation(
                        pickedObject->transformation()
                        * Matrix43f::rotation(camera->up(), displacement.x * objectAngularVelocity, pickedPoint)
                        * Matrix43f::rotation(camera->right(), displacement.y * objectAngularVelocity, pickedPoint)
                    );
                    renderer->invalidateScene();
                    renderer->start();
                }
            });
            connect(viewLabel, &Label::mouseButtonReleased, [=](Qt::MouseButton mousebutton) {
                if (mousebutton == Qt::RightButton) {
                    restoreOverrideCursor();
                }
                if (mousebutton == Qt::LeftButton) {
                    pickedObject = nullptr;
                }
            });
            return viewLabel;
        }());
        mainWindow->addDockWidget(Qt::LeftDockWidgetArea, [&]{
            QDockWidget* rendererSettingsDockWidget = new QDockWidget(mainWindow);
            rendererSettingsDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
            rendererSettingsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
            rendererSettingsDockWidget->setTitleBarWidget(new QWidget(rendererSettingsDockWidget));
            rendererSettingsDockWidget->setWidget([&]{
                QWidget* renderSettingsWidget = new QWidget();
                QFormLayout* renderSettingsFormLayout = new QFormLayout(renderSettingsWidget);
                renderSettingsFormLayout->addRow("Photon count", [&]{
                    QSpinBox* photonCountSpinBox = new QSpinBox();
                    photonCountSpinBox->setRange(1000, 1000000);
                    photonCountSpinBox->setValue(renderer->photonCount());
                    connect(photonCountSpinBox, &QSpinBox::editingFinished, [=]{
                        renderer->abort();
                        renderer->setPhotonCount(photonCountSpinBox->value());
                        renderer->invalidateScene();
                        renderer->start();
                    });
                    return photonCountSpinBox;
                }());
                renderSettingsFormLayout->addRow("Photon gather count", [&]{
                    QSpinBox* photonGatherCountSpinBox = new QSpinBox();
                    photonGatherCountSpinBox->setRange(1, 10000);
                    photonGatherCountSpinBox->setValue(renderer->photonGatherCount());
                    connect(photonGatherCountSpinBox, &QSpinBox::editingFinished, [=]{
                        renderer->abort();
                        renderer->setPhotonGatherCount(photonGatherCountSpinBox->value());
                        renderer->invalidateCamera();
                        renderer->start();
                    });
                    return photonGatherCountSpinBox;
                }());
                renderSettingsFormLayout->addRow([&]{
                    QPushButton* repaintPushButton = new QPushButton();
                    repaintPushButton->setText("Repaint");
                    connect(repaintPushButton, &QPushButton::pressed, [=]{
                        renderer->abort();
                        renderer->invalidateScene();
                        renderer->start();
                    });
                    return repaintPushButton;
                }());
                renderSettingsWidget->setLayout(renderSettingsFormLayout);
                return renderSettingsWidget;
            }());
            return rendererSettingsDockWidget;
        }());
        mainWindow->setStatusBar([&]{
            QStatusBar* statusBar = new QStatusBar(mainWindow);
            //statusBar->addPermanentWidget([&]{
            //    QLabel* fps = new QLabel(statusBar);
            //    connect(frameFrequencyMeter, &FrequencyMeter::frequencyChanged, [=](float frequency) {
            //        fps->setText(QString("FPS: %1").arg(frequency, 0, 'g', 2));
            //    });
            //    return fps;
            //}());
            return statusBar;
        }());
        mainWindow->resize(800, 600);
        mainWindow->show();
    }
}