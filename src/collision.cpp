#include "collision.h"
#include "utils.h"

Collision::Collision(WorldItem *a, WorldItem *b, btManifoldPoint &touchPoint):
    m_a(a)
  , m_b(b)
  , m_collisionPoint(touchPoint)
{
}
///////////////////////////////////////////////
/// \brief Collision::getCollidee
/// \param me
/// \return
/// This returns the 'other' collision WorldItem than the one passed.
/// NULL if the item passed is not one of the pairs
WorldItem *Collision::getCollidee(WorldItem *me) {
    if (me == m_a)
        return m_b;
    if (me == m_b)
        return m_a;
    return NULL;
}

QVector3D Collision::getCollisionPoint() {
    return bt2QtVector3D(m_collisionPoint.getPositionWorldOnA());
}

QVector3D Collision::getNormalOnB() {
    return bt2QtVector3D(m_collisionPoint.m_normalWorldOnB);
}


