#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>
#include "motionmanager.h"

class WorldItem; // Mutual link with WorldItem from worlditem.h

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionShapes/btCollisionShape.h"


#include "transform.h"

class Physics : public MotionManager
{
    Q_OBJECT
public:
    explicit Physics(btCollisionShape* shape, btScalar mass, WorldItem* parent);
    btRigidBody* getRigidBody(){ return m_body; }
    void setTransformVelocity(Transform t, QVector3D v=QVector3D(0,0,0));
    void setTransform(Transform t);
    Transform getTransform();
    void setVelocity(QVector3D v);
    QVector3D getVelocity();

    void addToWorld(World* world);
    void removeFromWorld(World *world);
    bool hasMotion();

signals:

public slots:

private:
    btCollisionShape* m_shape;
    btRigidBody* m_body;
    WorldItem* m_worldItem;
};

#endif // PHYSICS_H
