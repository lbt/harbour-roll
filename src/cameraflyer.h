#ifndef CAMERAFLYER_H
#define CAMERAFLYER_H

#include "basemotion.h"
#include <QtSensors/QAccelerometer>

class CameraFlyer : public BaseMotion
{
    Q_OBJECT
public:
    explicit CameraFlyer(WorldItem* parent = 0);

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
