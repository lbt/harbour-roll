#include "lightorbiter.h"
#include "utils.h"

LightOrbiter::LightOrbiter(QObject *parent) :
    LightManager(parent)
{
}
LightOrbiter::LightOrbiter(qreal radius, qreal theta, qreal phi,
                           qreal vr, qreal vth, qreal vph,
                           qreal rx, qreal ry, qreal rz,
                           qreal rxv, qreal ryv, qreal rzv,
                           QVector3D scale,
                           QObject *parent) :
    LightManager(parent)
  , m_r(radius)     // location
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
  , m_active(false)

{
    update(0);
}

QMatrix4x4 LightOrbiter::matrix(QMatrix4x4 input){
    input.translate(m_x, m_y, m_z);
    input.scale(m_scale);
    input.rotate(m_rx*PI180, 1,0,0);
    input.rotate(m_ry*PI180, 0,1,0);
    input.rotate(m_rz*PI180, 0,0,1);
    return input;
}

void LightOrbiter::randomise() {
    m_r = rnd(1.0);
    m_theta = rnd(PI);
    m_phi = rnd(PI);
    m_vr = rnd(1.0);
    m_vth = rnd(1.0);
    m_vph = rnd(1.0);
    update(0);
}

