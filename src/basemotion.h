#ifndef BASEMOTION_H
#define BASEMOTION_H

#include <QObject>
#include "transform.h"

class WorldItem;
class World;

class BaseMotion : public QObject
{
    Q_OBJECT
public:
    explicit BaseMotion(WorldItem* parent = 0);
    virtual void addToWorld(World *world) { Q_UNUSED(world); }
    virtual void removeFromWorld(World *world) { Q_UNUSED(world); }

    virtual void setTransformVelocity(Transform t, QVector3D v=QVector3D(0,0,0));
    virtual void setTransform(Transform t) {  m_transform = t; }
    virtual Transform getTransform() const { return m_transform; }
    virtual void setVelocity(QVector3D v) { m_velocity = v; }
    virtual QVector3D getVelocity() const { return m_velocity; }

    // use getTransform to allow non m_transform solutions to work
    virtual QVector3D right() const { return getTransform().right(); }
    virtual QVector3D up() const { return getTransform().up(); }
    virtual QVector3D forward() const { return getTransform().forward(); }
    virtual QVector3D at() const { return getTransform().at(); }


    virtual void lookAt(QVector3D go, QVector3D target, QVector3D up);

    virtual void randomise(QVector3D bottomBackLeft=QVector3D(-5,-5,-5), QVector3D topFrontRight=QVector3D(5,5,5));
    virtual void runStep(int deltaTms);

signals:

public slots:

protected:
    Transform m_transform;
    QVector3D m_velocity;
};

#endif // BASEMOTION_H
