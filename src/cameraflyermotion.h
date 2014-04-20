#ifndef CAMERAFLYERMOTION_H
#define CAMERAFLYERMOTION_H

#include "basemotion.h"
#include <QtSensors/QAccelerometer>

class CameraFlyerMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit CameraFlyerMotion(WorldItem* parent = 0);

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

#endif // CAMERAFLYERMOTION_H
