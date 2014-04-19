#include "cameramanager.h"
#include "world.h"
#include <QDebug>

// Thanks to : https://stevehazen.wordpress.com/2010/02/15/matrix-basics-how-to-step-away-from-storing-an-orientation-as-3-angles/

CameraManager::CameraManager(QString name, Display display) :
    WorldItem(name)
  , m_display(display)
{ }

void CameraManager::addToWorld(World *world)
{
    if (inWorld()) return;
    world->lock();
    world->add(this);
    WorldItem::addToWorld(world);
    world->unlock();
}

void CameraManager::removeFromWorld()
{
    if (! inWorld()) return;
    World* world = dynamic_cast<World*>(parent());
    world->lock();
    world->remove(this);
    WorldItem::removeFromWorld();
    world->unlock();
}

void CameraManager::lookAt(QVector3D go, QVector3D target, QVector3D up)
{
    Transform t;
    t.lookAt(go, target, up);
    t = t.inverted();
    setTransform(t);
}

void CameraManager::activate()
{
    World* world = dynamic_cast<World*>(parent());
    if (world) world->setActiveCamera(this);
}

QMatrix4x4 CameraManager::projViewMatrix() const {
    QMatrix4x4 projMatrix;
    projMatrix.perspective(m_display.m_fov, ((float)m_display.m_screenWidth/(float)m_display.m_screenHeight), 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed
    return projMatrix * getTransform().inverted(); // The view matrix is the inverse of the camera position.
}

void CameraManager::reset()
{
    setTransform(Transform());
}

void CameraManager::update(int deltaTms)
{
    Q_UNUSED(deltaTms);
}
