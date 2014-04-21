#include "lookatmotion.h"
#include "worlditem.h"

LookAtMotion::LookAtMotion(WorldItem *parent) :
    BaseMotion(parent)
  , m_wi(NULL)
  , m_lookAt(0,0,0)
  , m_up(0,0,1)
{
}

void LookAtMotion::runStep(int deltaTms)
{
    Q_UNUSED(deltaTms);
    if (m_wi)
        m_lookAt = m_wi->getTransform().at();
}

Transform LookAtMotion::getTransform(Transform current) const {
    QVector3D at = current.at();
    Transform here;
    here.lookAt(at, m_lookAt, m_up);
    here = here.inverted();
    if (m_next)
        here = m_next->getTransform(here);

    //qDebug() << "LookAt Transform final " << here.at() << " looking at " << here.forward() << " up " << here.up() ;
    return here;
}
