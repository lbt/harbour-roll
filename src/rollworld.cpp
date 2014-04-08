#include "rollworld.h"

//////////////////////////////////////////////////////////
/// \brief RollWorld::RollWorld
/// \param parent
///
RollWorld::RollWorld(QObject *parent) :
    World(parent)
{
}


void RollWorld::createRunner() {
    // Setup a worker Thread to do the bullet calcs
    m_runner = new RollRunner(this);
}

QStringList RollWorld::getTrackNames()
{
    return QStringList();
}

void RollWorld::setGravity(float x,float y,float z)
{
    dynamicsWorld->setGravity(btVector3(-x*10, -y*10, -z*10));
}

#define START    btVector3(2.0,-0.0,0)
void RollWorld::runStep(int ms) {
    World::runStep(ms);

    // Do our local collision detection until World has a mechanism
    RollWorld::ContactResultCallback result;

    dynamicsWorld->contactTest(m_ball->physics()->getRigidBody(), result);
    for (const btCollisionObject *obj : result.getContacts()) {
        if (obj == m_ball->physics()->getRigidBody()) continue;
        if (obj == m_floor->physics()->getRigidBody()) {
            qDebug() << "Hit the floor";
//            m_ball->collision(m_floor);
            btMotionState *motion;
            motion = m_ball->physics()->getRigidBody()->getMotionState();
            btTransform pos;
            motion->getWorldTransform(pos);
            pos.setOrigin(START);
            motion->setWorldTransform(pos);
            m_ball->physics()->getRigidBody()->setMotionState(motion);
            m_ball->physics()->getRigidBody()->setLinearVelocity(btVector3(0,0,0));
        }
    }
}
//////////////////////////////////////////////////////////
/// \brief RollRunner::RollRunner
/// \param w
/// \param parent
///
RollRunner::RollRunner(RollWorld *w, QObject *parent):
    WorldRunner(w, parent)
  , m_rollWorld(w)
  , m_gravity(true)
  , m_fly(false)
{
}

void RollRunner::setup()
{
    WorldRunner::setup();
    m_sensor.start();
}

void RollRunner::gravity(bool state)
{
    m_gravity = state;
}

void RollRunner::setDebugDraw(bool state)
{
    if (state)
        m_rollWorld->m_debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    else
        m_rollWorld->m_debugDrawer.setDebugMode(0);
}

void RollRunner::runStep() {
    if (m_gravity) {
        //        if (!m_fly) {
        QAccelerometerReading *reading = m_sensor.reading();
        m_rollWorld->setGravity(reading->x(), reading->y(), reading->z());
        //        }
    } else {
        m_rollWorld->setGravity(0, 0, 0);
    }

    WorldRunner::runStep();
}
