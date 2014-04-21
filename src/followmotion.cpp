#include "followmotion.h"
#include "worlditem.h"

FollowMotion::FollowMotion(WorldItem *parent) :
    BaseMotion(parent)
  , m_offset(0,0,1)
{
}

void FollowMotion::runStep(int deltaTms)
{
    Q_UNUSED(deltaTms);
    QVector3D itemPos = m_wi->getTransform().at();
    // This follows the ball from 'behind' using the velocity to determine behind.
    // But that's not as good as it could be when it's rolling up and down slopes.
    //    QVector3D v = r.column(1).toVector3D() * -0.5;
    // Transform does not (yet?) have velocity component
    //    v.setZ(18);
    //    qDebug() << "v " << v;

    m_transform.setToIdentity();
    m_transform.lookAt(itemPos + m_offset, itemPos, QVector3D(0, 0, -1));
    m_transform = m_transform.inverted();
}

void FollowMotion::follow(WorldItem* wi, QVector3D offset)
{
    m_wi = wi;
    m_offset = offset;
    runStep(0);
}
