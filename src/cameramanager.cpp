#include "cameramanager.h"
#include <QDebug>

// Thanks to : https://stevehazen.wordpress.com/2010/02/15/matrix-basics-how-to-step-away-from-storing-an-orientation-as-3-angles/

#define PI 3.14159
#define HEIGHT 960
#define WIDTH 540

CameraManager::CameraManager(QObject *parent) :
    QObject(parent)
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

        QMatrix4x4 r;
        QVector3D prev_up = up();
        QVector3D prev_right = right();
        QVector3D prev_fwd = forward();
        r.rotate((m_touchX - x)/WIDTH * 2, prev_up); // rotate around up
        r.rotate((y - m_touchY)/HEIGHT * 2, prev_right); // rotate around right

        QVector4D p = position(); // store position
        m_camera = r * m_camera; // rotate
        setPosition(p); // restore position

    } else { // movement bottom half
        QMatrix4x4 t;
        t.translate(forward()*0.5*(y - m_touchY)/HEIGHT); // movement
        m_camera = t * m_camera; // move

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/WIDTH * 2,  up()) ; // rotation vector
        QVector4D p = position(); // store position
        m_camera = r* m_camera; // rotate
        setPosition(p); // restore position
    }

}

void CameraManager::release() {
    m_pressed = false;
    m_touchX = 0;
    m_touchY = 0;
}

#define FOVY 50
#define ASPECT ((float)screenWidth()/(float)screenHeight())

QMatrix4x4 CameraManager::projViewMatrix() {
    QMatrix4x4 projMatrix;
    projMatrix.perspective(FOVY, ASPECT, 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed

    return projMatrix * m_camera.inverted(); // The view matrix is the inverse of the camera position.
//    QMatrix4x4 viewMatrix;
//    viewMatrix.lookAt(at(), at() - forward(),  up());
//    return projMatrix * viewMatrix;
}

// Extract up, right, forward and position vectors
QVector3D CameraManager::right() { // x
    return m_camera.column(0).toVector3D();
}
QVector3D CameraManager::up() { // y
    return m_camera.column(1).toVector3D();
}
QVector3D CameraManager::forward() { // z
    return m_camera.column(2).toVector3D();
}
QVector3D CameraManager::at() { // position
    return m_camera.column(3).toVector3D();
}

// private for store/restore position during non-translating rotation
QVector4D CameraManager::position() {
    return m_camera.column(3);
}
void CameraManager::setPosition(QVector4D pos) {
    m_camera.setColumn(3, pos);
}

void CameraManager::reset()
{
    m_camera=QMatrix4x4();
    // Normally any messing with the orientation axis would require all 3 to be kept in sync.
//    m_camera.setColumn(0, QVector4D(1,  0,  0,  0));
//    m_camera.setColumn(1, QVector4D(0,  1,  0,  0));
//    m_camera.setColumn(2, QVector4D(0,  0,  1,  0));
//    m_camera.setColumn(3, QVector4D(0,  0,  10, 1));
    m_camera.translate(QVector3D(0,  0,  10));
    qDebug() << "camera set to " << m_camera;
    m_touchY=0;
    m_touchX=0;
    m_pressed=false;

}
