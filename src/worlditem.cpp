#include "worlditem.h"
#include "shader.h"
#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(QString name) :
    QObject(NULL)
  , m_physics(NULL)
{
    setObjectName(name);
}

bool WorldItem::inWorld(){
    if (parent()) {
        qDebug() << "In a World (can't add anything)";
        return true;
    } else return false;
}

void WorldItem::add(Physics *p)
{
    if (inWorld()) return;
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
    if (inWorld()) return;
    if (r) {
        m_renderables << r;
    } else {
        qDebug() << "Refusing to add null";
    }
}
///////////////////
/// \brief WorldItem::shaderList
/// \return
/// This is used by the World to group render calls by enabled Shader
///
QList<Shader*> WorldItem::shaderList() {
    QList<Shader*> shaders;
    for (Renderable* r: m_renderables) {
        Shader* s = r->getShader();
        if (!s) {
            qDebug() <<"No shader for Renderable " << r->objectName() << " in " << objectName();
        } else {
            shaders << s;
        }
    }
    return shaders;
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

void WorldItem::addToWorld(World *world)
{
    if (inWorld()) return;
    world->lock();
    world->add(this, shaderList());
    if (m_physics) world->add(m_physics);
    setParent(world);
    world->unlock();
}

void WorldItem::removeFromWorld()
{
    if (! inWorld()) return;
    World* world = dynamic_cast<World*>(parent());
    world->lock();
    if (m_physics) world->remove(m_physics);
    world->remove(this, shaderList());
    setParent(NULL);
    world->unlock();
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


