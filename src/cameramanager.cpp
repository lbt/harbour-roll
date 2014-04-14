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
    m_sensor.start();
}

void CameraManager::touch(qreal x, qreal y) {
    if (! m_pressed) {
        m_touchX = x;
        m_touchY = y;
        m_pressed = true;
    }

    if (m_touchY < HEIGHT/2) { // rotation top half

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/WIDTH * 2, up()); // rotate around up
        r.rotate((y - m_touchY)/HEIGHT * 2, right()); // rotate around right

        m_camera.rotateOnly(r);
    } else { // movement bottom half
        QMatrix4x4 t;
        t.translate(m_camera.forward()*0.5*(y - m_touchY)/HEIGHT); // movement
        QMatrix4x4 t2 = t * m_camera; // move
        m_camera = t * m_camera; // move

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/WIDTH * 2,  up()) ; // rotation vector
        m_camera.rotateOnly(r);
    }

}

void CameraManager::follow(Transform itemTransform)
{
    QVector3D p = itemTransform.at();
// This follows the ball from 'behind' using the velocity to determine behind.
// But that's not as good as it could be when it's rolling up and down slopes.
//    QVector3D v = r.column(1).toVector3D() * -0.5;
    // Transform does not (yet?) have velocity component
//    v.setZ(18);
//    qDebug() << "v " << v;

    QVector3D v = QVector3D(0.2,0.2,15);

    m_camera=Transform();
    m_camera.lookAt(p + v, p, QVector3D(0, 0, -1));
    m_camera = m_camera.inverted();

//    qDebug() << "At " << m_camera.at() <<" Looking towards " << m_camera.forward();
}

void CameraManager::updatePosition() {
    QAccelerometerReading *reading = m_sensor.reading();
    if (reading) {
//        QMatrix4x4 t;
//        t.translate(forward()*(reading->y()/10.0)); // movement
//        m_camera = t * m_camera; // move

        QMatrix4x4 r;
//        r.rotate(reading->x()/5.0,  forward()) ; // rotation vector
//        r.rotate(reading->y()/5.0,  right()) ; // rotation vector
        m_camera.rotateOnly(r);
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

void CameraManager::reset()
{
    m_camera=Transform();
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
