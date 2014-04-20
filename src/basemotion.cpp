#include "basemotion.h"
#include "worlditem.h"
#include "world.h"
#include "utils.h"

BaseMotion::BaseMotion(WorldItem *parent) :
    QObject(parent)
{
}

void BaseMotion::setTransformVelocity(Transform t, QVector3D v)
{
    m_transform = t;
    m_velocity = v;
}

void BaseMotion::lookAt(QVector3D go, QVector3D target, QVector3D up)
{
    Transform t;
    t.lookAt(go, target, up);
    t = t.inverted();
    setTransform(t); // don't write to m_transform so subclasses will respect this
}

void BaseMotion::randomise(QVector3D bottomBackLeft, QVector3D topFrontRight)
{
    QVector3D &min = bottomBackLeft;
    QVector3D &max = topFrontRight;
    QVector3D pos(rnd(max.x()-min.x())+min.x(),
                  rnd(max.y()-min.y())+min.y(),
                  rnd(max.z()-min.z())+min.z() );
    Transform t;
    t.translate(pos);
    setTransform(t);
}

void BaseMotion::runStep(int deltaTms) {
    m_transform.translate(m_velocity * (deltaTms/1000.0));
}
