#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionShapes/btCollisionShape.h"

#include "worlditem.h"
class WorldItem; // Mutual link with WorldItem from worlditem.h

#include "transform.h"

class Physics : public QObject
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

    void addToPhysicsWorld(btDiscreteDynamicsWorld* world);
    void removeFromPhysicsWorld(btDiscreteDynamicsWorld *world);
    bool ownsMotion();

signals:

public slots:

private:
    btCollisionShape* m_shape;
    btRigidBody* m_body;
    WorldItem* m_worldItem;
};

#endif // PHYSICS_H
