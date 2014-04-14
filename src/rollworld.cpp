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
{}


void RollWorld::createRunner() {
    // Setup a worker Thread to do the bullet calcs
    qDebug() << "Making a RollRunner";
    m_runner = new RollRunner(this);
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

#define START    btVector3(2.0,-0.0,0)
void RollWorld::runStep(int ms) {

    Transform ballTransform = m_ball->getTransform();
    m_camera->follow(ballTransform);
    // m_camera->update(ms);

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
