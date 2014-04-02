#include "worldobject.h"
#include <QDebug>

WorldObject::WorldObject(BiMesh* bimesh, btRigidBody *body, QObject *parent) :
    QObject(parent)
  , m_bimesh(bimesh)
  , m_rigidBody(body)
  , m_hit(false)
{
    m_hitTimer.setSingleShot(true);
    m_hitTimer.setInterval(500);
    connect(&m_hitTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
}

void WorldObject::setHit(bool hit) {
    if (!hit && !m_hit) return;
    if (hit && m_hit) return;
    m_hit =  hit;
    if (hit) {
        m_hitTimer.start();
    } else {
        m_hitTimer.stop();
    }
}

void WorldObject::render(GLProgram *p) {
    if (! m_bimesh) return;
    m_rigidBody = getRigidBody();

    if (m_rigidBody && m_rigidBody->getMotionState())
    {
        btTransform trans;
        m_rigidBody->getMotionState()->getWorldTransform(trans);
        QMatrix4x4  pos = bt2QMatrix4x4(&trans);
        p->setUniformValue(p->getU("worldMatrixU"), pos);

        // If glowing...
        if (m_hit) {
            p->setUniformValue(p->getU("Glow"), QVector4D(1.0, -0.2, -0.2, 0.0));
        } else {
            p->setUniformValue(p->getU("Glow"), QVector4D(0.0, 0.0, 0.0, 0.0));
        }
        m_bimesh->render(p);
    }
}


