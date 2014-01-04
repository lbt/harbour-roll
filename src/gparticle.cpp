#include "gparticle.h"
#include <math.h>
#include <QtDebug>

// phi is 'around' the equator
GParticle::GParticle(float radius, float theta, float phi,
                     float vr, float vth, float vph,
                     float rx, float ry, float rz,
                     float rxv, float ryv, float rzv,
                     float scale) :
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
    update(0, {0.0,0.0,0.0,0.0}, 0); // set xyz
}

#define RADIUS 5.0
#define PI 3.14159265
#define PI180 (180/PI)
#define DECEL 0.99
void GParticle::update(float deltaTime, Wind w, float wind_r){

#define MAXV 10.0
    if (w.x != 0 ) {
        // we have wind and are at the front of the bubble
        if ((m_x > w.x-wind_r) && (m_x < w.x+wind_r) &&
                (m_y > w.y-wind_r) && (m_y < w.y+wind_r)) {
            //            qDebug() << " hit XY (" << m_x << "," << m_y << ") matched within " << wind_r
            //                     << "(" << w.x << "," << w.y << ")";
            //            // in the zone
            //            qDebug() << " pre V (" << m_vph << "," << m_vth<<") ("<<w.vx<<","<<w.vy<<")";
            if (m_z > 0) {
                m_vph += w.vx;
                m_vth += w.vy;
            } else {
                m_vph += w.vx;
                m_vth += w.vy;
            }
            if (m_vph > MAXV) m_vph = MAXV;
            if (m_vph < -MAXV) m_vph = -MAXV;
            if (m_vth > MAXV) m_vth = MAXV;
            if (m_vth < -MAXV) m_vth = -MAXV;
            //            qDebug() << " post V (" << m_vph << "," << m_vth << ")";
        }
    }
    m_vph *= DECEL;
    m_vth *= DECEL;
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

    // All velocities calculated; determine location in cartesian
    m_x = m_r * cos(m_theta) * cos(m_phi);
    m_z = m_r * cos(m_theta) * sin(m_phi);
    m_y = m_r * sin(m_theta);
    //    qDebug() << "x y z (" << m_x <<"," << m_y<<"," << m_z<<")"
    //             << " r t p (" << m_r <<"," << m_theta<<"," << m_phi<<")"
    //             << " rx ry rz (" << m_rx <<"," << m_ry<<"," << m_rz<<")";
}

float GParticle::x() { return m_x;}
float GParticle::y() { return m_y;}
float GParticle::z() { return m_z;}

QMatrix4x4 GParticle::matrix(QMatrix4x4 input){
    input.translate(m_x, m_y, m_z);
    input.scale(m_scale,m_scale,m_scale);
    input.rotate(m_rx*PI180, 1,0,0);
    input.rotate(m_ry*PI180, 0,1,0);
    input.rotate(m_rz*PI180, 0,0,1);
    return input;
}


