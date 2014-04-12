#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "bullet/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet/BulletCollision/Gimpact/btGImpactShape.h"

#include <assimp/scene.h>

#include "worlditem.h"
class WorldItem; // Mutual link with WorldItem from worlditem.h

class Physics : public QObject
{
    Q_OBJECT
public:
    explicit Physics(btCollisionShape* shape, btScalar mass, WorldItem* parent);
    btRigidBody* getRigidBody(){ return m_body; }
    void setInWorld(bool state) { m_inWorld = state; }
    void setPos(btVector3 pos, btVector3 velocity=btVector3(0,0,0));

signals:

public slots:

private:
    btCollisionShape* m_shape;
    btRigidBody* m_body;
    WorldItem* m_worldItem;
    bool m_inWorld;
};

#endif // PHYSICS_H
