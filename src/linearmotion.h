#ifndef LINEARMOTION_H
#define LINEARMOTION_H
#include "basemotion.h"

class LinearMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit LinearMotion(WorldItem *parent = 0);

    virtual void setTransformVelocity(Transform t, QVector3D v=QVector3D(0,0,0));
    virtual void setTransform(Transform t) {  m_transform = t; }
    virtual void setVelocity(QVector3D v) { m_velocity = v; }
    virtual QVector3D getVelocity() const { return m_velocity; }

    void runStep(int deltaTms);
    void randomise(QVector3D bottomBackLeft, QVector3D topFrontRight);
signals:

public slots:

};

#endif // LINEARMOTION_H
