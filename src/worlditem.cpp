#include "worlditem.h"
#include "shader.h"
#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(QString name) :
    QObject(NULL)
  , m_physics(NULL)
  , m_transform()
  , m_velocity()
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
    if (! m_physics){
        m_physics = p;
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
    if (m_physics && m_physics->ownsMotion()) m_physics->setTransform(t);
    m_transform = t;
}

Transform WorldItem::getTransform() {
    if (m_physics && m_physics->ownsMotion()) {
        return m_physics->getTransform();
    } else {
        return m_transform;
    }
}

void WorldItem::setVelocity(QVector3D v) {
    // try and set physics velocity and fall back to local
    if (m_physics && m_physics->ownsMotion()) m_physics->setVelocity(v);
    m_velocity = v;
}

QVector3D WorldItem::getVelocity() {
    if (m_physics && m_physics->ownsMotion()) {
        return m_physics->getVelocity();
    } else {
        return m_velocity;
    }
}

////
/// \brief WorldItem::render
/// \param p - This is used to select the correct VAO for the currently active GLProgram
///
void WorldItem::render(const Shader *activeProgram) {
    // Get the transform from physics or as-stored.
    m_transform = getTransform();

    for (auto r: m_renderables) {
        r->render(activeProgram, m_transform);
    }
}


