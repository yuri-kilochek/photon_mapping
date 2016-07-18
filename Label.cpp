#include "Label.hpp"

namespace photon_mapping {
    Label::Label(QWidget* parent)
        : QLabel(parent)
        , m_mousePosition(0)
    {
        setMouseTracking(true);
        setAlignment(Qt::AlignCenter);
        setScaledContents(true);
    }

    QSize Label::minimumSizeHint() const {
        return QSize();
    }

    bool Label::isKeyPressed(Qt::Key key) const {
        return m_keysPressed.count(key);
    }

    bool Label::isMouseButtonPressed(Qt::MouseButton button) const {
        return m_mouseButtonsPressed.count(button);
    }

    Point2i* Label::mousePosition() const {
        return m_mousePosition;
    }

    void Label::resizeEvent(QResizeEvent* event) {
        emit resized(Extent2i(event->size().width(), event->size().height()));
        event->accept();
    }

    void Label::keyPressEvent(QKeyEvent* event) {
        Qt::Key key = (Qt::Key)event->key();
        m_keysPressed.insert(key);
        emit keyPressed(key);
        event->accept();
    }

    void Label::keyReleaseEvent(QKeyEvent* event) {
        Qt::Key key = (Qt::Key)event->key();
        m_keysPressed.erase(key);
        emit keyReleased(key);
        event->accept();
    }

    void Label::mousePressEvent(QMouseEvent* event) {
        Qt::MouseButton mouseButton = event->button();
        m_mouseButtonsPressed.insert(mouseButton);
        emit mouseButtonPressed(mouseButton);
        event->accept();
    }

    void Label::mouseReleaseEvent(QMouseEvent* event) {
        Qt::MouseButton mouseButton = event->button();
        m_mouseButtonsPressed.erase(mouseButton);
        emit mouseButtonReleased(mouseButton);
        event->accept();
    }

    void Label::mouseDoubleClickEvent(QMouseEvent* event) {
        Qt::MouseButton mouseButton = event->button();
        emit mouseButtonDoubleClicked(mouseButton);
        event->accept();
    }

    void Label::mouseMoveEvent(QMouseEvent* event) {
        if (m_mousePosition) {
            Point2i oldMousePosition = *m_mousePosition;
            *m_mousePosition = Point2i(event->pos().x(), event->pos().y());
            Vector2i displacement = *m_mousePosition - oldMousePosition;
            if (sqrLength(displacement) > 0) {
                emit mouseMoved(*m_mousePosition, displacement);
            }
        } else {
            m_mousePosition = new Point2i(event->pos().x(), event->pos().y());
            emit mouseEntered(*m_mousePosition);
        }
        event->accept();
    }
    void Label::leaveEvent(QEvent* event) {
        delete m_mousePosition;
        m_mousePosition = 0;
        event->accept();
    }
}