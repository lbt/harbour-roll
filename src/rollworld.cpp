#include "rollworld.h"

//////////////////////////////////////////////////////////
/// \brief RollWorld::RollWorld
/// \param parent
///
RollWorld::RollWorld(QObject *parent) :
    World(parent)
  , m_ball(NULL)
  , m_floor(NULL)
  , m_gravity(true)
  , m_dbgMode(0)
{}

void RollWorld::setup() {
    World::setup();
    m_sensor.start();
}

QStringList RollWorld::getTrackNames()
{
    return QStringList();
}

void RollWorld::setGravity(float x,float y,float z)
{
    dynamicsWorld->setGravity(btVector3(-x*10, -y*10, -z*10));
}

void RollWorld::setDebugDraw(int mode)
{
    qDebug() << "Set debug "<< m_dbgMode << " to toggle" << mode;
    m_dbgMode ^= mode;
    qDebug() << " now "<< m_dbgMode;
// http://www.continuousphysics.com/Bullet/BulletFull/btIDebugDraw_8h_source.html
// DBG_DrawWireframe = 1,
// DBG_DrawAabb=2,
// DBG_FastWireframe = (1<<13),
// DBG_DrawNormals = (1<<14),
    m_debugDrawer.setDebugMode(m_dbgMode);
}


void RollWorld::runStep(int ms) {

    if (m_gravity) {
        //        if (!m_fly) {
        QAccelerometerReading *reading = m_sensor.reading();
        setGravity(reading->x(), reading->y(), reading->z());
        //        }
    } else {
        setGravity(0, 0, 0);
    }

    // For dynamic camera following a curve
    // m_camera->update(ms);
    // m_cammanager.updatePosition();

    World::runStep(ms);

    // Do our local collision detection until World has a mechanism
    RollWorld::ContactResultCallback result;
    Q_ASSERT(m_ball->physicsMotion() != NULL);
    Q_ASSERT(m_floor->physicsMotion() != NULL);
    dynamicsWorld->contactTest(m_ball->physicsMotion()->getRigidBody(), result);
    for (const btCollisionObject *obj : result.getContacts()) {
        if (obj == m_ball->physicsMotion()->getRigidBody()) continue;
        if (obj == m_floor->physicsMotion()->getRigidBody()) {
            qDebug() << "Hit the floor";
            m_ball->reset();
        }
    }
}

