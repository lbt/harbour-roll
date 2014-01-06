#include "gparticle2.h"
#include <math.h>
#include <QtDebug>

qreal clamp(qreal d, qreal min, qreal max) {
    const qreal t = d < min ? min : d;
    return t > max ? max : t;
}

// phi is 'around' the equator
GParticle2::GParticle2(float radius, float theta, float phi,
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
  , m_vx(0)
  , m_vy(0)
  , m_vz(0)
  , m_elastic(0.9-(rand() %50)/100.0)
{
    m_x = m_r * cos(m_theta) * cos(m_phi);
    m_z = m_r * cos(m_theta) * sin(m_phi);
    m_y = m_r * sin(m_theta);
    update(0, {0.0,0.0,0.0,0.0}, 0, {0,0,0}); // set xyz
    qDebug() << "XYZ  (" << m_x << "," << m_y<<"," << m_z << ")";

}

#define RADIUS 5.0
#define PI 3.14159265
#define PI180 (180/PI)
#define DECEL 0.999
void GParticle2 ::update(float deltaTime, Wind w, float wind_r, Accel a){

#define MAXV 10.0
    if (w.x != 0 ) {
        // we have wind
        if ((m_x > w.x-wind_r) && (m_x < w.x+wind_r) &&
                (m_y > w.y-wind_r) && (m_y < w.y+wind_r)) {
            if (m_z > 0) {
                m_vph += w.vx;
                m_vth += w.vy;
            } else {
                m_vph += w.vx;
                m_vth += w.vy;
            }
            m_vph=clamp(m_vph, -MAXV, MAXV);
            m_vth=clamp(m_vth, -MAXV, MAXV);
        }
    }
//    m_vph *= DECEL;
//    m_vth *= DECEL;
//    m_r     += m_vr  * deltaTime;
    // bounce off RADIUS and origin
//    if (m_r > RADIUS) {
//        m_r = RADIUS - (m_r - RADIUS);
//        m_vr = -m_vr;
//    }
//    if (m_r < RADIUS/2.0) {
//        m_r = RADIUS/2.0 + (RADIUS/2.0 -m_r);
//        m_vr = -m_vr;
//    }
//    m_theta += m_vth * deltaTime;
//    if (m_theta > 2*PI) m_theta -= 2*PI;
//    m_phi   += m_vph * deltaTime;
//    if (m_phi > 2*PI) m_phi -= 2*PI;
    // Tumbling
    m_rx += m_rxv * deltaTime;
    if (m_rx > 2*PI) m_rx -= 2*PI;
    m_ry += m_ryv * deltaTime;
    if (m_ry > 2*PI) m_ry -= 2*PI;
    m_rz += m_rzv * deltaTime;
    if (m_rz > 2*PI) m_rz -= 2*PI;

    // All velocities calculated; determine location in cartesian
    //    m_x = m_r * cos(m_theta) * cos(m_phi);
    //    m_z = m_r * cos(m_theta) * sin(m_phi);
    //    m_y = m_r * sin(m_theta);
    //    qDebug() << "orig    r = " << m_r << " theta = " << m_theta <<" phi = " << m_phi;
    //    m_r = sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    //    m_theta = asin(m_y/m_r);
    //    m_phi = atan2(m_y, m_x);
    //    qDebug() << "reverse r = " << m_r << " theta = " << m_theta <<" phi = " << m_phi;

    m_vx -= a.x * deltaTime * 10;
    m_vy -= a.y * deltaTime * 10;
    m_vz -= a.z * deltaTime * 10;

    m_x += m_vx * deltaTime;
    m_z += m_vz * deltaTime;
    m_y += m_vy * deltaTime;


    // Calculate wall bounces
    if (m_x > RADIUS) {
        m_x = RADIUS;
        m_vx = -m_vx * m_elastic;
    } else if (m_x < -RADIUS) {
        m_x = -RADIUS;
        m_vx = -m_vx * m_elastic;
    }
    if (m_y > RADIUS) {
        m_y = RADIUS;
        m_vy = -m_vy * m_elastic;
    } else if (m_y < -RADIUS) {
        m_y = -RADIUS;
        m_vy = -m_vy * m_elastic;
    }
    if (m_z > RADIUS) {
        m_z = RADIUS;
        m_vz = -m_vz * m_elastic;
    } else if (m_z < -RADIUS) {
        m_z = -RADIUS;
        m_vz = -m_vz * m_elastic;
    }

//    qDebug() << "XYZ  (" << m_x << "," << m_y<<"," << m_z << ")";
//    qDebug() << "XYZ v(" << m_vx << "," << m_vy<<"," << m_vz << ")";
//    qDebug() << "XYZ a(" << a.x << "," << a.y<<"," << a.z << ")";

//    m_r = sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
//    m_theta = asin(m_y/m_r);
//    m_phi = atan(m_y/m_x);
}

float GParticle2::x() { return m_x;}
float GParticle2::y() { return m_y;}
float GParticle2::z() { return m_z;}

QMatrix4x4 GParticle2::matrix(QMatrix4x4 input){
    input.translate(m_x, m_y, m_z);
    input.scale(m_scale,m_scale,m_scale);
    input.rotate(m_rx*PI180, 1,0,0);
    input.rotate(m_ry*PI180, 0,1,0);
    input.rotate(m_rz*PI180, 0,0,1);
    return input;
}


