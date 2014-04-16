#include "worlditem.h"
#include "shader.h"
#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(QString name, World *parent) :
    QObject(parent)
  , m_world(parent)
  , m_physics(NULL)
{
    setObjectName(name);
}

void WorldItem::add(Physics *p)
{
    if (! m_physics) {
        if (p) {
            m_physics = p;
        } else {
            qDebug() << "Refusing to add null";
        }
    } else {
        qDebug() << "Already have a physics. Not adding another";
    }
}
void WorldItem::add(Renderable *r)
{
    if (r) {
        m_renderables << r;
    } else {
        qDebug() << "Refusing to add null";
    }
}

void WorldItem::addToWorld()
{
    if (m_physics) {
        m_world->add(m_physics);
        m_physics->setInWorld(true);
    }
    m_world->add(this);
}

void WorldItem::leaveWorld()
{
    m_world->remove(m_physics);
    m_physics->setInWorld(false);
    m_world->remove(this);
}


void WorldItem::setupGL(){
    qDebug() << "Setup GL";
    for (auto r : m_renderables){
        r->setupGL();
    }
}

Transform WorldItem::getTransform() {
    if (m_physics && m_physics->getRigidBody() && m_physics->getRigidBody()->getMotionState())
    {
        btTransform trans;
        m_physics->getRigidBody()->getMotionState()->getWorldTransform(trans);
        return bt2QMatrix4x4(&trans);
    } else {
        return m_transform;
    }
}
void WorldItem::setTransform(QMatrix4x4 t) {
    if (m_physics && m_physics->getRigidBody() && m_physics->getRigidBody()->getMotionState()) {
        btTransform transform = Qt2btTransform(&t);
        btMotionState* motion = m_physics->getRigidBody()->getMotionState();
        motion->setWorldTransform(transform);
        m_physics->getRigidBody()->setMotionState(motion);
    } else {
        m_transform = t;
    }
}

void WorldItem::setVelocity(QVector3D v) {
    if (m_physics && m_physics->getRigidBody()) {
        btVector3 velocity = Qt2btVector3(v);
        m_physics->getRigidBody()->setLinearVelocity(velocity);
    } else {
        m_velocity = v;
    }
}
QVector3D WorldItem::getVelocity() {
    if (m_physics && m_physics->getRigidBody()) {
        return bt2QtVector3D(m_physics->getRigidBody()->getLinearVelocity());
    } else {
        return m_velocity;
    }
}

////
/// \brief WorldItem::render
/// \param p - This is used to select the correct VAO for the currently active GLProgram
///
void WorldItem::render(const Shader *activeProgram) {

    // If we're a physics body then we update using physics.
    // Other moving classes will need to update pos some other way
    if (m_physics && m_physics->getRigidBody() && m_physics->getRigidBody()->getMotionState())
    {
        btTransform trans;
        m_physics->getRigidBody()->getMotionState()->getWorldTransform(trans);
        m_transform = bt2QMatrix4x4(&trans);
    }

    for (auto r: m_renderables) {
        r->render(activeProgram, m_transform);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


