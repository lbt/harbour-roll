#include "rollworld.h"

//////////////////////////////////////////////////////////
/// \brief RollWorld::RollWorld
/// \param parent
///
RollWorld::RollWorld(QObject *parent) :
    World(parent)
  , m_ball(NULL)
  , m_floor(NULL)
  , m_camera(NULL)
  , m_ballStartPos(5.5, -5.5, 1)
  , m_gravity(true)
  , m_fly(true)
{}

void RollWorld::setup() {
    World::setup();
    m_sensor.start();
}

QMatrix4x4 RollWorld::getActiveCameraPVM()
{
    return m_camera->projViewMatrix();
}

QVector3D RollWorld::getActiveCameraAt()
{
    return m_camera->at();
}

QStringList RollWorld::getTrackNames()
{
    return QStringList();
}

void RollWorld::setGravity(float x,float y,float z)
{
    dynamicsWorld->setGravity(btVector3(-x*10, -y*10, -z*10));
}

void RollWorld::setDebugDraw(bool state)
{
    if (state)
        m_debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    else
        m_debugDrawer.setDebugMode(0);
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

    if (!m_fly) {
        //    Transform ballTransform = m_ball->getTransform();
        Transform ballTransform = Transform();
        ballTransform.translate(QVector3D(0,  0,  0));

        m_camera->follow(ballTransform);
    }

    // For dynamic camera following a curve
    // m_camera->update(ms);
    // m_cammanager.updatePosition();

    World::runStep(ms);

    // Do our local collision detection until World has a mechanism
    RollWorld::ContactResultCallback result;

    dynamicsWorld->contactTest(m_ball->physics()->getRigidBody(), result);
    for (const btCollisionObject *obj : result.getContacts()) {
        if (obj == m_ball->physics()->getRigidBody()) continue;
        if (obj == m_floor->physics()->getRigidBody()) {
            qDebug() << "Hit the floor";
            //            m_ball->collision(m_floor);
            Transform pos;
            pos.translate(m_ballStartPos);
            m_ball->setTransform(pos);
            m_ball->setVelocity(QVector3D());
        }
    }
}

