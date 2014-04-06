#include "physics.h"
#include "utils.h"
#include <QDebug>

Physics::Physics(btCollisionShape* shape, btVector3 pos, btScalar mass, WorldItem *parent) :
    QObject(parent)
  , m_shape(shape)
{
    /// Create Dynamic Objects
    btMatrix3x3 rot;
//    rot.setEulerZYX(90,0,0);  // Match the GL orientation

    btTransform startTransform(rot, pos);
    //    startTransform.setIdentity();
    //    startTransform.rotate();
    //    startTransform.setOrigin(pos);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);

    if (isDynamic)
        shape->calculateLocalInertia(mass,localInertia);

    qDebug() << "add physics ";
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
    rbInfo.m_friction=0.9;
    rbInfo.m_restitution=0.7;

    m_body = new btRigidBody(rbInfo);

    m_body->setUserPointer((void*)this);
}
