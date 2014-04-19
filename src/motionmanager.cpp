#include "motionmanager.h"
#include "worlditem.h"
#include "world.h"

MotionManager::MotionManager(WorldItem *parent) :
    QObject(parent)
{
}

void MotionManager::setTransformVelocity(Transform t, QVector3D v)
{
    m_transform = t;
    m_velocity = v;
}

void MotionManager::runStep(int deltaTms) {
    m_transform.translate(m_velocity * (deltaTms/1000.0));
}
