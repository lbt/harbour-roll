#include "physics.h"
#include "utils.h"

#include "bullet/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet/BulletCollision/Gimpact/btGImpactShape.h"

#include <QDebug>

Physics::Physics(btCollisionShape* shape, btScalar mass, WorldItem *parent):
    QObject(parent)
  , m_shape(shape)
{
    /// Create Dynamic Objects
    //    btMatrix3x3 rot;
    //    rot.setIdentity();
    //    rot.setEulerZYX(90,0,0);  // Match the GL orientation

    btTransform startTransform;
    startTransform.setIdentity();
    //    btTransform startTransform(rot, pos);
    //    startTransform.rotate(rot);
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

bool Physics::ownsMotion(){
    return (m_body && m_body->getMotionState());
}

void Physics::setTransformVelocity(Transform t, QVector3D v)
{
    if (ownsMotion()) {
        btTransform transform = Qt2btTransform(&t);
        btVector3 velocity = Qt2btVector3(v);
        btMotionState* motion = m_body->getMotionState();
        motion->setWorldTransform(transform);
        getRigidBody()->setMotionState(motion);
        getRigidBody()->setLinearVelocity(velocity);
    } else qDebug() << "Tried to set pos, velocity when motion not owned";
}

void Physics::setTransform(Transform t)
{
    if (ownsMotion()) {
        btTransform transform = Qt2btTransform(&t);
        btMotionState* motion = m_body->getMotionState();
        motion->setWorldTransform(transform);
        m_body->setMotionState(motion);
    } else qDebug() << "Tried to set Transform when motion not owned";
}

Transform Physics::getTransform() {
    if (ownsMotion()) {
        btTransform trans;
        m_body->getMotionState()->getWorldTransform(trans);
        return Transform(bt2QMatrix4x4(&trans));
    } else {
        qDebug() << "Tried to get Transform when motion not owned";
        return Transform();
    }
}
void Physics::setVelocity(QVector3D v)
{
    if (ownsMotion()) {
        btVector3 velocity = Qt2btVector3(v);
        m_body->setLinearVelocity(velocity);
    } else qDebug() << "Tried to set velocity when motion not owned";
}

QVector3D Physics::getVelocity()
{
    if (ownsMotion()) {
        return bt2QtVector3D(m_body->getLinearVelocity());
    } else {
        qDebug() << "Tried to get velocity when motion not owned";
        return QVector3D();
    }
}

void Physics::addToPhysicsWorld(btDiscreteDynamicsWorld *world)
{
    world->addRigidBody(m_body);
}

void Physics::removeFromPhysicsWorld(btDiscreteDynamicsWorld *world)
{
    world->removeRigidBody(m_body);
}
