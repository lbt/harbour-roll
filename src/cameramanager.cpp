#include "cameramanager.h"
#include "world.h"
#include <QDebug>
#include <sailfishapp.h>

// Thanks to : https://stevehazen.wordpress.com/2010/02/15/matrix-basics-how-to-step-away-from-storing-an-orientation-as-3-angles/

CameraManager::CameraManager(QString name, BaseMotion *motion, Display display) :
    WorldItem(name, motion)
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

GLProgram* CameraManager::c_program_debug = 0;

void CameraManager::debugRender(QMatrix4x4 projViewMatrix){
    if (! c_program_debug)
        c_program_debug = new GLProgram(SailfishApp::pathTo("light_vert.glsl"), SailfishApp::pathTo("light_frag.glsl"));
    // Renders each light as a large point
    QMatrix4x4 worldMatrix; // null
    c_program_debug->bind();
    c_program_debug->setUniformValue(c_program_debug->getU("projViewMatrixU"), projViewMatrix);
    c_program_debug->setUniformValue(c_program_debug->getU("worldMatrixU"), worldMatrix);
    glEnableVertexAttribArray(c_program_debug->getA("posA"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    c_program_debug->setUniformValue(c_program_debug->getU("colU"), QVector4D(1, .2, .2, 1.0));
    QVector3D pos = getTransform().at();
    GLfloat point[] = {pos.x(), pos.y(), pos.z(), 0.0, 0.0, 0.0 };
    glVertexAttribPointer(c_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, point);
    // it's nice to have the line drawn from the origin
    glDrawArrays(GL_POINTS, 0, 1);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(c_program_debug->getA("posA"));
    //    qDebug() << "Rendering Light at " << getTransform().at();
}
