#ifndef COLLISION_H
#define COLLISION_H

#include "worlditem.h"
#include <QVector3D>
#include "bullet/BulletCollision/NarrowPhaseCollision/btManifoldPoint.h"

class Collision
{
public:
    Collision(WorldItem* a, WorldItem* b, btManifoldPoint &touchPoint);

    WorldItem*  getCollidee(WorldItem *me);
    WorldItem*  getColliderA() { return m_a; }
    WorldItem*  getColliderB() { return m_b; }
    QVector3D getCollisionPoint();
    QVector3D getNormalOnA();
    QVector3D getNormalOnB();

private:

private:
    WorldItem* m_a;
    WorldItem* m_b;
    btManifoldPoint &m_collisionPoint;
//    btManifoldPoint m_btManifoldPoint; // This may be needed in the future
};

#endif // COLLISION_H
