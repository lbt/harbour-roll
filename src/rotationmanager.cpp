#include "rotationmanager.h"
#include <QDebug>

#define PI 3.14159
#define HEIGHT 960
#define WIDTH 540

RotationManager::RotationManager(QObject *parent) :
    QObject(parent)
  , m_depth(-10)
  , m_phi(0)
  , m_theta(0)
  , m_touchY(0)
  , m_touchX(0)
  , m_pressed(false)
{
}

void RotationManager::touch(qreal x, qreal y) {
    if (! m_pressed) {
        m_touchX = x;
        m_touchY = y;
        m_pressed = true;
    }
    m_x = x;
    m_y = y;

    if (m_touchY < HEIGHT/2) { // rotation top half
        m_theta += (m_touchY - m_y)/HEIGHT * 10;
        m_phi += (m_touchX - m_x)/WIDTH * 10;
    } else { // movement bottom half
        m_depth += (m_touchY - m_y)/HEIGHT * 2;
        m_phi += (m_touchX - m_x)/WIDTH * 10;
    }

}

void RotationManager::release() {
    m_pressed = false;
    m_touchX = 0;
    m_touchY = 0;
}

QMatrix4x4 RotationManager::transform(QMatrix4x4 v) {
    v.translate(0, 0, m_depth);
    v.rotate(m_theta, 1, 0, 0);
    v.rotate(m_phi, 0, 1, 0);
    return v;
}
