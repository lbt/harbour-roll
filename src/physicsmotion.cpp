#include "physicsmotion.h"
#include "utils.h"

#include "world.h"
#include "worlditem.h"

#include "bullet/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet/BulletCollision/Gimpact/btGImpactShape.h"

#include <QDebug>

PhysicsMotion::PhysicsMotion(btCollisionShape* shape, btScalar mass, WorldItem *parent):
    BaseMotion(parent)
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

    qDebug() << "add PhysicsMotion ";
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
    rbInfo.m_friction=0.9;
    rbInfo.m_restitution=0.7;

    m_body = new btRigidBody(rbInfo);

    m_body->setUserPointer((void*)this);
}

bool PhysicsMotion::hasMotion() const {
    return (m_body && m_body->getMotionState());
}

void PhysicsMotion::setTransformVelocity(Transform t, QVector3D v)
{
    if (hasMotion()) {
        btTransform transform = Qt2btTransform(&t);
        btVector3 velocity = Qt2btVector3(v);
        btMotionState* motion = m_body->getMotionState();
        motion->setWorldTransform(transform);
        getRigidBody()->setMotionState(motion);
        getRigidBody()->setLinearVelocity(velocity);
    } else qDebug() << "Tried to set pos, velocity without a MotionState";
}

void PhysicsMotion::setTransform(Transform t)
{
    if (hasMotion()) {
        btTransform transform = Qt2btTransform(&t);
        btMotionState* motion = m_body->getMotionState();
        motion->setWorldTransform(transform);
        m_body->setMotionState(motion);
    } else qDebug() << "Tried to set Transform without a MotionState";
}

Transform PhysicsMotion::getTransform(Transform current) const
{
    Transform here;
    if (hasMotion()) {
        btTransform trans;
        m_body->getMotionState()->getWorldTransform(trans);
        here = current * Transform(bt2QMatrix4x4(&trans));
    } else {
        qDebug() << "Tried to get Transform without a MotionState";
    }
    if (m_next)
        return m_next->getTransform(here);
    return here;
}
void PhysicsMotion::setVelocity(QVector3D v)
{
    if (hasMotion()) {
        btVector3 velocity = Qt2btVector3(v);
        m_body->setLinearVelocity(velocity);
    } else qDebug() << "Tried to set velocity without a MotionState";
}

QVector3D PhysicsMotion::getVelocity() const
{
    if (hasMotion()) {
        return bt2QtVector3D(m_body->getLinearVelocity());
    } else {
        qDebug() << "Tried to get velocity without a MotionState";
        return QVector3D();
    }
}

void PhysicsMotion::addToWorld(World *world)
{
    world->lock();
    world->dynamicsWorld->addRigidBody(m_body);
    world->unlock();
}

void PhysicsMotion::removeFromWorld(World *world)
{
    world->lock();
    world->dynamicsWorld->removeRigidBody(m_body);
    world->unlock();
}
