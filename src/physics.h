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
    explicit Physics(btCollisionShape* shape, btVector3 pos, btScalar mass, WorldItem* parent);
    btRigidBody* getRigidBody(){ return m_body; }

signals:

public slots:

private:
    btCollisionShape* m_shape;
    btRigidBody* m_body;
    WorldItem* m_worldItem;
};

#endif // PHYSICS_H
