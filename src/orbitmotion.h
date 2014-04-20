#ifndef ORBITMOTION_H
#define ORBITMOTION_H
#include "basemotion.h"
#include <QMatrix4x4>
#include <QVector3D>

class OrbitMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit OrbitMotion(WorldItem *parent = 0);

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

#endif // ORBITMOTION_H
