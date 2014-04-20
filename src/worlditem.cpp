#include "worlditem.h"
#include "renderable.h"
#include "shader.h"
#include "world.h"
#include "basemotion.h"
#include "physicsmotion.h"

#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(QString name) :
    QObject(NULL)
  , m_motion(NULL)
{
    setObjectName(name);
}

bool WorldItem::inWorld(){
    if (parent()) {
        qDebug() << "In a World (can't add anything)";
        return true;
    } else return false;
}

BaseMotion* WorldItem::setMotion(BaseMotion *m)
{
    BaseMotion* old = m_motion;
    m_motion = m;
    return old;
}
void WorldItem::addRenderable(Renderable *r)
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
    Q_ASSERT(m_motion != NULL);
    world->lock();
    world->add(this, shaderList());
    m_motion->addToWorld(world);
    setParent(world);
    world->unlock();
}

void WorldItem::removeFromWorld()
{
    if (! inWorld()) return;
    World* world = dynamic_cast<World*>(parent());
    world->lock();
    m_motion->removeFromWorld(world);
    world->remove(this, shaderList());
    setParent(NULL);
    world->unlock();
}

////
/// \brief WorldItem::render
/// \param p - This is used to select the correct VAO for the currently active GLProgram
///
void WorldItem::render(const Shader *activeProgram) {
    // Get the transform from PhysicsMotion or as-stored.
    Transform t = getTransform();

    for (auto r: m_renderables) {
        r->render(activeProgram, t);
    }
}


