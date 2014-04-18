#include "cameramanager.h"
#include <QDebug>

// Thanks to : https://stevehazen.wordpress.com/2010/02/15/matrix-basics-how-to-step-away-from-storing-an-orientation-as-3-angles/

CameraManager::CameraManager(QString name, Display display, QObject *parent) :
    QObject(parent)
  , m_display(display)
{
    setObjectName(name);
}

void CameraManager::setDisplay(int w, int h, qreal fov)
{
    m_display=Display(w, h, fov);
}

void CameraManager::lookAt(QVector3D go, QVector3D target, QVector3D up)
{
    m_camera=Transform();
    m_camera.lookAt(go, target, up);
    m_camera = m_camera.inverted();
}

void CameraManager::update(int deltaTms)
{
    Q_UNUSED(deltaTms);
}

QMatrix4x4 CameraManager::projViewMatrix() {
    QMatrix4x4 projMatrix;
    projMatrix.perspective(m_display.m_fov, ((float)m_display.m_screenWidth/(float)m_display.m_screenHeight), 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed
    return projMatrix * m_camera.inverted(); // The view matrix is the inverse of the camera position.
}

void CameraManager::reset()
{
    m_camera=Transform();
}
