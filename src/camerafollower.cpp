#include "camerafollower.h"

CameraFollower::CameraFollower(QString name, Display display, QObject *parent) :
    CameraManager(name, display, parent)
{
}

void CameraFollower::update(int deltaTms)
{
    Q_UNUSED(deltaTms);
    QVector3D itemPos = m_wi->getTransform().at();
    // This follows the ball from 'behind' using the velocity to determine behind.
    // But that's not as good as it could be when it's rolling up and down slopes.
    //    QVector3D v = r.column(1).toVector3D() * -0.5;
    // Transform does not (yet?) have velocity component
    //    v.setZ(18);
    //    qDebug() << "v " << v;

    QVector3D cameraOffset = QVector3D(0.2,0.2, m_dist);
    m_camera=Transform();
    m_camera.lookAt(itemPos + cameraOffset, itemPos, QVector3D(0, 0, -1));
    m_camera = m_camera.inverted();
}

void CameraFollower::follow(WorldItem* wi, float dist)
{
    m_wi = wi;
    m_dist = dist;
    update(0);
}