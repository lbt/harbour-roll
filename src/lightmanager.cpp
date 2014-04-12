#include "lightmanager.h"
#include <math.h>

#define RADIUS 1.0
#define DECEL 0.999
#define MAXV 10.0

#include "utils.h"

LightManager::LightManager(qreal radius, qreal theta, qreal phi,
                           qreal vr, qreal vth, qreal vph,
                           qreal rx, qreal ry, qreal rz,
                           qreal rxv, qreal ryv, qreal rzv,
                           QVector3D scale) :
    m_r(radius)     // location
  , m_theta(theta)  // location
  , m_phi(phi)      // location
  , m_vr(vr)        // radial location velocity
  , m_vth(vth)      // theta location velocity
  , m_vph(vph)      // phi location velocity
  , m_rx(rx)        // orientation x
  , m_ry(ry)        // orientation y
  , m_rz(rz)        // orientation z
  , m_rxv(rxv)      // orientation x velocity
  , m_ryv(ryv)      // orientation y velocity
  , m_rzv(rzv)      // orientation z velocity
  , m_scale(scale)  // scale (xyz)
{
    update(0);
}

LightManager::LightManager() :
    m_r(rnd(1.0))     // location
  , m_theta(rnd(PI))  // location
  , m_phi(rnd(PI))      // location
  , m_vr(rnd(1.0))        // radial location velocity
  , m_vth(rnd(1.0))      // theta location velocity
  , m_vph(rnd(1.0))      // phi location velocity
  , m_rx(0)        // orientation x
  , m_ry(0)        // orientation y
  , m_rz(0)        // orientation z
  , m_rxv(0)      // orientation x velocity
  , m_ryv(0)      // orientation y velocity
  , m_rzv(0)      // orientation z velocity
  , m_scale()  // scale (xyz)
{
}

void LightManager::update(int ms)
{
    if (!m_active) return;
    qreal deltaTime = ms/1000.0;
    m_r     += m_vr  * deltaTime;
    // bounce off RADIUS and origin
    if (m_r > RADIUS) {
        m_r = RADIUS - (m_r - RADIUS);
        m_vr = -m_vr;
    }
    if (m_r < RADIUS/2.0) {
        m_r = RADIUS/2.0 + (RADIUS/2.0 -m_r);
        m_vr = -m_vr;
    }
    m_theta += m_vth * deltaTime;
    if (m_theta > 2*PI) m_theta -= 2*PI;
    m_phi   += m_vph * deltaTime;
    if (m_phi > 2*PI) m_phi -= 2*PI;
    // Tumbling
    m_rx += m_rxv * deltaTime;
    if (m_rx > 2*PI) m_rx -= 2*PI;
    m_ry += m_ryv * deltaTime;
    if (m_ry > 2*PI) m_ry -= 2*PI;
    m_rz += m_rzv * deltaTime;
    if (m_rz > 2*PI) m_rz -= 2*PI;

    // Apply an X/Y/Z scaling to

    // All velocities calculated; determine location in cartesian
    m_x = m_r * cos(m_theta) * cos(m_phi);
    m_z = m_r * cos(m_theta) * sin(m_phi);
    m_y = m_r * sin(m_theta);

    //    qDebug() << "x y z (" << m_x <<"," << m_y<<"," << m_z<<")"
    //             << " r t p (" << m_r <<"," << m_theta<<"," << m_phi<<")"
    //             << " vr vt vp (" << m_vr <<"," << m_vth <<"," << m_vph <<")"
    //             << " t:" << deltaTime;
    //             << " rx ry rz (" << m_rx <<"," << m_ry<<"," << m_rz<<")";

}

QMatrix4x4 LightManager::matrix(QMatrix4x4 input){
    input.translate(m_x, m_y, m_z);
    input.scale(m_scale);
    input.rotate(m_rx*PI180, 1,0,0);
    input.rotate(m_ry*PI180, 0,1,0);
    input.rotate(m_rz*PI180, 0,0,1);
    return input;
}

QVector3D LightManager::pos()
{
    return QVector3D(m_x, m_y, m_z) * m_scale;
}
QVector3D LightManager::scale() const
{
    return m_scale;
}

void LightManager::setScale(const QVector3D &scale)
{
    m_scale = scale;
}

void LightManager::randomise() {
    m_r = rnd(1.0);
    m_theta = rnd(PI);
    m_phi = rnd(PI);
    m_vr = rnd(1.0);
    m_vth = rnd(1.0);
    m_vph = rnd(1.0);
    update(0);
}

