#include "basemotion.h"
#include "worlditem.h"
#include "world.h"
#include "utils.h"

BaseMotion::BaseMotion(WorldItem *parent) :
    QObject(parent)
  , m_next(NULL)
  , m_worldItem(NULL)
{
}

void BaseMotion::setMotion(BaseMotion *m)
{
    delete m_next;
    m_next = m;
    // m_next will be deleted as a QObject child
    m_next->setParent(this);
    m_next->setWorldItem(m_worldItem);
}

Transform BaseMotion::getTransform(Transform current) const {
    Transform here = current * m_transform;
    if (m_next)
        here = m_next->getTransform(here);
    //    qDebug() << this->metaObject()->className()<< ":Transform "<< here << " at " << here.at() << " looking at " << here.forward() ;
    return here;
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
    // This will break if/when Motions are shared
    if (m_next) m_next->runStep(deltaTms);
}

