#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include <QObject>
#include "transform.h"
#include <QtSensors/QAccelerometer>

class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = 0);

    bool isActive() const { return m_pressed; }
    void reset();

    QVector3D right() { return m_camera.right(); }
    QVector3D up() { return m_camera.up(); }
    QVector3D forward() { return m_camera.forward(); }
    QVector3D at() { return m_camera.at(); }
    QMatrix4x4 projViewMatrix();
    int screenWidth() { return 540; }
    int screenHeight() { return 960; }

    void updatePosition();
signals:

public slots:
    void release(); // no longer in use
    void touch(qreal x, qreal y); // current position
    void follow(Transform itemTransform);

private:
    QVector4D position();
    void setPosition(QVector4D position);
    QAccelerometer m_sensor;

    bool m_pressed;
    qreal m_touchX;
    qreal m_touchY;
    Transform m_camera;
};

#endif // ROTATIONMANAGER_H
