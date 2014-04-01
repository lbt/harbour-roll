#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include <QObject>
#include <QMatrix4x4>
#include <QtSensors/QAccelerometer>

class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = 0);

    bool isActive() const { return m_pressed; }
    void reset();

    QVector3D right();
    QVector3D up();
    QVector3D forward();
    QVector3D at();
    QMatrix4x4 projViewMatrix();
    int screenWidth() { return 540; }
    int screenHeight() { return 960; }

    void updatePosition();
signals:

public slots:
    void release(); // no longer in use
    void touch(qreal x, qreal y); // current position
    void follow(QMatrix4x4 r);

private:
    QVector4D position();
    void setPosition(QVector4D position);
    QAccelerometer m_sensor;

    bool m_pressed;
    qreal m_touchX;
    qreal m_touchY;
    QMatrix4x4 m_camera;
};

#endif // ROTATIONMANAGER_H
