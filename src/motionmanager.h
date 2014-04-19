#ifndef MOTIONMANAGER_H
#define MOTIONMANAGER_H

#include <QObject>
#include "transform.h"

class WorldItem;
class World;

class MotionManager : public QObject
{
    Q_OBJECT
public:
    explicit MotionManager(WorldItem* parent = 0);
    virtual void addToWorld(World *world) { Q_UNUSED(world); }
    virtual void removeFromWorld(World *world) { Q_UNUSED(world); }

    virtual void setTransformVelocity(Transform t, QVector3D v=QVector3D(0,0,0));
    virtual void setTransform(Transform t) {  m_transform = t; }
    virtual Transform getTransform() const { return m_transform; }
    virtual void setVelocity(QVector3D v) { m_velocity = v; }
    virtual QVector3D getVelocity() const { return m_velocity; }

    virtual void runStep(int deltaTms);

signals:

public slots:

protected:
    Transform m_transform;
    QVector3D m_velocity;
};

#endif // MOTIONMANAGER_H
