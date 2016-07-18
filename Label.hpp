#ifndef PHOTON_MAPPING_LABEL_HPP
#define PHOTON_MAPPING_LABEL_HPP

#include <set>

#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

#include "Extent.hpp"
#include "Point.hpp"
#include "Vector.hpp"

namespace photon_mapping {
    class Label
        : public QLabel
    {
        Q_OBJECT
        public:
            Label(QWidget* parent = 0);

            virtual QSize minimumSizeHint() const;

            bool isKeyPressed(Qt::Key key) const;
            bool isMouseButtonPressed(Qt::MouseButton button) const;
            Point2i* mousePosition() const;

        signals:
            void resized(Extent2i const& extent);
            void keyPressed(Qt::Key key);
            void keyReleased(Qt::Key key);
            void mouseButtonPressed(Qt::MouseButton mouseButton);
            void mouseButtonReleased(Qt::MouseButton mouseButton);
            void mouseButtonDoubleClicked(Qt::MouseButton mouseButton);
            void mouseEntered(Point2i const& position);
            void mouseMoved(Point2i const& position, Vector2i const& displacement);
            void mouseLeft();

        protected:
            void resizeEvent(QResizeEvent* event);
            void keyPressEvent(QKeyEvent* event);
            void keyReleaseEvent(QKeyEvent* event);
            void mousePressEvent(QMouseEvent* event);
            void mouseReleaseEvent(QMouseEvent* event);
            void mouseDoubleClickEvent(QMouseEvent* event);
            void mouseMoveEvent(QMouseEvent* event);
            void leaveEvent(QEvent* event);

        private:
            std::set<Qt::Key> m_keysPressed;
            std::set<Qt::MouseButton> m_mouseButtonsPressed;
            Point2i* m_mousePosition;
    };
}

#endif