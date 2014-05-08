#include "worlditem.h"
#include "renderable.h"
#include "shader.h"
#include "world.h"
#include "basemotion.h"
#include "physicsmotion.h"

#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(QString name, BaseMotion *motion) :
    QObject(NULL)
  , m_renderables()
  , m_motion(motion)
  , m_transform()
  , m_collisionType(NO_COLLISONS)
{
    if (m_motion) m_motion->setWorldItem(this);
    setObjectName(name);
}

bool WorldItem::inWorld(){
    if (parent()) {
        return true;
    } else return false;
}

void WorldItem::setMotion(BaseMotion *m)
{
    delete m_motion;
    m_motion = m;
    // m_motion will be deleted as a QObject child
    m_motion->setParent(this);
    m_motion->setWorldItem(this);
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
void WorldItem::render(const Shader *activeProgram, render_pass pass) {
    // Get the transform from PhysicsMotion or as-stored.
    if (m_renderables.isEmpty()) return;
    Transform t = getTransform();
    for (Renderable* r: m_renderables) {
        if (pass == OPAQUE and r->isOpaque()){
            // qDebug() <<"OPAQUE: " << r->objectName();
            r->render(activeProgram, t);
        }
        if (pass == TRANSLUCENT and r->isTranslucent()){
            // qDebug() <<"TRANSLUCENT: " << r->objectName();
            r->render(activeProgram, t);
        }
    }
}

void WorldItem::collision(Collision& collision) {
    Q_UNUSED(collision);
}
