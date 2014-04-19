#ifndef CAMERAFLYER_H
#define CAMERAFLYER_H

#include "cameramanager.h"
#include <QtSensors/QAccelerometer>

class CameraFlyer : public CameraManager
{
    Q_OBJECT
public:
    explicit CameraFlyer(QString name, Display display, QObject *parent = 0);

    bool isActive() const { return m_pressed; }
    void updatePosition();
    void reset();

signals:

public slots:
    void touch(qreal x, qreal y);
    void release();

private:
    QAccelerometer m_sensor;

    bool m_pressed;
    qreal m_touchX;
    qreal m_touchY;
};

#endif // CAMERAFLYER_H
