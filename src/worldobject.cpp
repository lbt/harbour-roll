#include "worldobject.h"

WorldObject::WorldObject(BiMesh* bimesh, btRigidBody* body, QObject *parent) :
    QObject(parent)
  , m_bimesh(bimesh)
  , m_rigidBody(body)
{
}

void WorldObject::render(GLProgram *p) {
    m_rigidBody = getRigidBody();

    if (m_rigidBody && m_rigidBody->getMotionState())
    {
        btTransform trans;
        m_rigidBody->getMotionState()->getWorldTransform(trans);
        QMatrix4x4  pos = bt2QMatrix4x4(&trans);
        p->setUniformValue(p->getU("worldMatrixU"), pos);

        // If glowing...

        m_bimesh->render(p);
    }
}
