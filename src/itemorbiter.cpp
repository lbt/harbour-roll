#include "itemorbiter.h"
#include "utils.h"

ItemOrbiter::ItemOrbiter(WorldItem *parent) :
    MotionManager(parent)
  , m_origin()
  , m_theta(0)
  , m_scale(1,1,1)
  , m_faceOrigin(false)
  , m_active(true)
{
    randomise();
}

void ItemOrbiter::randomise() {

    m_axis = QVector3D(1- rnd(2.0), 1-rnd(2.0), 1-rnd(2.0));
    m_radius = QVector3D(QVector3D::normal(m_axis, QVector3D(
                                               1-rnd(2.0),
                                               1-rnd(2.0),
                                               1-rnd(2.0))))
            * rnd(5.0);
    m_angularSpeed = rnd(20)+5;
}


///////////////////////////////
/// \brief setup
/// \param axis is the rotational axis
/// \param axisPlane defines a plane which defines the radius normal to the axis
/// \param radius length of the radius
/// \param angularSpeed
/// \param origin
/// \param faceOrigin
/// \param startTheta
///
void ItemOrbiter::setup(QVector3D axis, QVector3D axisPlane, qreal radius, qreal angularSpeed,
                        QVector3D origin, bool faceOrigin, qreal startTheta)
{
    m_origin = origin;
    m_axis = axis;
    m_radius = QVector3D::normal(axis, axisPlane)*radius;
    m_angularSpeed = angularSpeed;
    m_faceOrigin = faceOrigin;
    m_active = true;
    m_theta = startTheta;
}

void ItemOrbiter::runStep(int deltaTms)
{
    m_theta += m_angularSpeed * (deltaTms/1000.0);
    if (m_theta < 0) m_theta += 360;
    if (m_theta > 360) m_theta -= 360;

    Transform t;
    t.rotate(m_theta, m_axis);
    if (m_faceOrigin) m_transform.rotateOnly(t);
    QVector3D newPos = m_origin + (m_radius * t) * m_scale;
    m_transform.setTranslate(newPos);
    // qDebug() << "theta : " << m_theta << " transform:" << m_transform;
}

Transform ItemOrbiter::getTransform() const {
    return m_transform;
}
