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
        qDebug() << "Camera is " << m_camera;

    } else { // movement bottom half
        QMatrix4x4 t;
        t.translate(forward()*0.5*(y - m_touchY)/HEIGHT); // movement
        qDebug() << "Moving from " << position() << " along " << forward() << " size " << forward()*(y - m_touchY)/HEIGHT;
        qDebug() << "t is " << t;
        m_camera = t * m_camera; // move
        qDebug() << "Camera is now " << m_camera;

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/WIDTH * 2,  up()) ; // rotation vector
        QVector4D p = position(); // store position
        m_camera = r* m_camera; // rotate
        setPosition(p); // restore position
        qDebug() << "Camera is now " << m_camera;
    }

}

void CameraManager::release() {
    m_pressed = false;
    m_touchX = 0;
    m_touchY = 0;
}

#define FOVY 50
#define ASPECT (540.0/960.0)

QMatrix4x4 CameraManager::projViewMatrix() {
    QMatrix4x4 projMatrix;
    projMatrix.perspective(FOVY, ASPECT, 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed

    QMatrix4x4 viewMatrix;
//    viewMatrix.rotate(m_theta, 1, 0, 0);
//    viewMatrix.rotate(m_phi, 0, 1, 0);
//    viewMatrix.translate(-m_pos);

    viewMatrix.lookAt(at(), at()+forward()*10,  up());

//    return projMatrix * viewMatrix;
    return projMatrix * m_camera.inverted();
//    return projMatrix * m_camera;
}

// Extract up, right, forward and position vectors
QVector3D CameraManager::right() { // x
    return m_camera.column(0).toVector3D();
//    return m_camera.inverted() * QVector4D(1,0,0,1).toVector3D();
}
QVector3D CameraManager::up() { // y
    return m_camera.column(1).toVector3D();
//    return m_camera.inverted() * QVector4D(0,1,0,0).toVector3D();
}
QVector3D CameraManager::forward() { // z
    return m_camera.column(2).toVector3D();
//    return m_camera.inverted() * QVector4D(0,0,1,0).toVector3D();
}
QVector3D CameraManager::at() { // position
    return m_camera.column(3).toVector3D();
}
QVector4D CameraManager::position() { // position
    return m_camera.column(3);
}
void CameraManager::setPosition(QVector4D pos) { // position
    m_camera.setColumn(3, pos);
}

void CameraManager::reset()
{
    m_camera=QMatrix4x4();
    // Normally any messing with the orientation axis would require all 3 to be kept in sync.
    m_camera.setColumn(0, QVector4D(1,  0,  0,  0));
    m_camera.setColumn(1, QVector4D(0,  1,  0,  0));
    m_camera.setColumn(2, QVector4D(0,  0,  1,  0));
    m_camera.setColumn(3, QVector4D(0,  0,  10, 1));
    qDebug() << "camera set to " << m_camera;
    m_touchY=0;
    m_touchX=0;
    m_pressed=false;

    QMatrix4x4 a;
    QMatrix4x4 b;
    qDebug() << "a is " << a;
    a.rotate(90, QVector3D(1, 1, 0) );
    qDebug() << "after rotate a is " << a;
    a.translate(QVector3D(2, 1, 2));
    qDebug() << "after translate a is " << a;
    a.rotate(30, QVector3D(2, 1, -5) );
    qDebug() << "after rotate a is " << a;
    b.translate(QVector3D(0, -1, 8));
    qDebug() << "b is " << b;
    qDebug() << "a*b is " << a*b;
    a *= b;
    qDebug() << "a*=b is " << a;
}
