#include "cameraflyer.h"

CameraFlyer::CameraFlyer(QString name, Display display, QObject *parent) :
    CameraManager(name, display, parent)
{
    m_sensor.start();
}

void CameraFlyer::touch(qreal x, qreal y) {
    if (! m_pressed) {
        m_touchX = x;
        m_touchY = y;
        m_pressed = true;
    }
    if (m_touchY < m_display.m_screenHeight/2) { // rotation top half

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/m_display.m_screenWidth * 2, up()); // rotate around up
        r.rotate((y - m_touchY)/m_display.m_screenHeight * 2, right()); // rotate around right

        m_camera.rotateOnly(r);
    } else { // movement bottom half
        QMatrix4x4 t;
        t.translate(m_camera.forward()*0.5*(y - m_touchY)/m_display.m_screenHeight); // movement
        m_camera = t * m_camera; // move

        QMatrix4x4 r;
        r.rotate((m_touchX - x)/m_display.m_screenWidth * 2,  up()) ; // rotation vector
        m_camera.rotateOnly(r);
    }

}

void CameraFlyer::updatePosition() {
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

void CameraFlyer::release() {
    m_pressed = false;
    m_touchX = 0;
    m_touchY = 0;
}

void CameraFlyer::reset()
{
    m_camera=Transform();
    // Normally any messing with the orientation axis would require all 3 to be kept in sync.
    //    m_camera.setColumn(0, QVector4D(1,  0,  0,  0));
    //    m_camera.setColumn(1, QVector4D(0,  1,  0,  0));
    //    m_camera.setColumn(2, QVector4D(0,  0,  1,  0));
    //    m_camera.setColumn(3, QVector4D(0,  0,  10, 1));
    m_camera.translate(QVector3D(0,  0,  32));
    qDebug() << "camera set to " << m_camera;
    m_touchY=0;
    m_touchX=0;
    m_pressed=false;
}