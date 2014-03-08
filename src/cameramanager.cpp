#include "cameramanager.h"
#include <QDebug>

#define PI 3.14159
#define HEIGHT 960
#define WIDTH 540

CameraManager::CameraManager(QObject *parent) :
    QObject(parent)
  , m_pos(0,0,10)
  , m_phi(0)
  , m_theta(0)
  , m_touchY(0)
  , m_touchX(0)
  , m_pressed(false)
{
    reset();
}

void CameraManager::touch(qreal x, qreal y) {
    if (! m_pressed) {
        m_touchX = x;
        m_touchY = y;
        m_pressed = true;
    }

    if (m_touchY < HEIGHT/2) { // rotation top half
        m_theta += (m_touchY - y)/HEIGHT * 10;
        m_phi += (m_touchX - x)/WIDTH * 10;
    } else { // movement bottom half
        m_pos.setZ(m_pos.z() - (m_touchY - y)/HEIGHT * 0.2);
        m_pos.setX(m_pos.x() + (m_touchX - x)/WIDTH * 0.2) ;
    }

}

void CameraManager::release() {
    m_pressed = false;
    m_touchX = 0;
    m_touchY = 0;
}

QMatrix4x4 CameraManager::transform(QMatrix4x4 v) {
    return v;
}

#define FOVY 50
#define ASPECT (540.0/960.0)

QMatrix4x4 CameraManager::projViewMatrix() {
    QMatrix4x4 projMatrix;
    projMatrix.perspective(FOVY, ASPECT, 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed

    QMatrix4x4 viewMatrix;
    viewMatrix.rotate(m_theta, 1, 0, 0);
    viewMatrix.rotate(m_phi, 0, 1, 0);
    viewMatrix.translate(-m_pos);
    return projMatrix * viewMatrix;
}

QVector3D CameraManager::at() { // position of the camera in the world
    return QVector3D(m_pos);
}

void CameraManager::reset()
{
    m_pos=QVector3D(0,0,10);
    m_phi=0;
    m_theta=0;
    m_touchY=0;
    m_touchX=0;
    m_pressed=false;
}
