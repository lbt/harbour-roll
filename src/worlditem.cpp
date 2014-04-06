#include "worlditem.h"
#include <QDebug>

#include "utils.h"

WorldItem::WorldItem(World *w, World *parent) :
    QObject(parent)
  , m_world(w)
{
}

void WorldItem::add(Physics *p)
{
    if (! m_physics) {
        m_physics = p;
        m_world->add(p);
    } else {
        qDebug() << "Already have a physics. Not adding another";
    }
}

////
/// \brief WorldItem::render
/// \param p - This is used to select the correct VAO for the currently active GLProgram
///
void WorldItem::render(GLProgram* activeProgram) {

    // If we're a physics body then we update using physics.
    // Other moving classes will need to update pos some other way
    if (m_physics && m_physics->getRigidBody() && m_physics->getRigidBody()->getMotionState())
    {
        btTransform trans;
        m_physics->getRigidBody()->getMotionState()->getWorldTransform(trans);
        m_pos = bt2QMatrix4x4(&trans);
    }

    for (auto r: m_renderables) {
        r->render(activeProgram);
    }
}


