#ifndef ITEMORBITER_H
#define ITEMORBITER_H
#include "basemotion.h"
#include <QMatrix4x4>
#include <QVector3D>

class ItemOrbiter : public BaseMotion
{
    Q_OBJECT
public:
    explicit ItemOrbiter(WorldItem *parent = 0);

    void setup(QVector3D axis, QVector3D axisPlane, qreal radius, qreal angularSpeed,
               QVector3D origin = QVector3D(), bool faceOrigin = false,
               qreal startTheta = 0);

    void setScale(QVector3D scale) { m_scale = scale; }
    void active(bool state) { m_active = state; }

    void runStep(int deltaTms);
    Transform getTransform() const;

signals:

public slots:
    void randomise();

private:
    QVector3D m_origin; // rotate around this point
    QVector3D m_axis;
    QVector3D m_radius;
    qreal m_angularSpeed;
    qreal m_theta;
    QVector3D m_scale;  // for making elipses
    bool m_faceOrigin;
    bool m_active;

};

#endif // ITEMORBITER_H
