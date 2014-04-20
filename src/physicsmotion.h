#ifndef PHYSICSMOTION_H
#define PHYSICSMOTION_H

#include <QObject>
#include "basemotion.h"

class WorldItem; // Mutual link with WorldItem from worlditem.h

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionShapes/btCollisionShape.h"


#include "transform.h"

class PhysicsMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit PhysicsMotion(btCollisionShape* shape, btScalar mass, WorldItem* parent);
    virtual btRigidBody* getRigidBody(){ return m_body; }
    virtual void setTransformVelocity(Transform t, QVector3D v=QVector3D(0,0,0));
    virtual void setTransform(Transform t);
    virtual Transform getTransform() const;
    virtual void setVelocity(QVector3D v);
    virtual QVector3D getVelocity() const;

    virtual void addToWorld(World* world);
    virtual void removeFromWorld(World *world);
    virtual bool hasMotion() const;

signals:

public slots:

protected:
    btCollisionShape* m_shape;
    btRigidBody* m_body;
    WorldItem* m_worldItem;
};

#endif // PHYSICSMOTION_H
