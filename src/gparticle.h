#ifndef GPARTICLE_H
#define GPARTICLE_H
#include <QMatrix4x4>

class GParticle
{
public:
    struct Wind {
        float x;
        float y;
        float vx;
        float vy;
    };

public:
    GParticle(float radius, float theta, float phi,
              float vr, float vth, float vph,
              float rx, float ry, float rz,
              float rvx, float ryv, float rzv,
              float scale);

    void update(float dt, Wind w);
    QMatrix4x4 matrix(QMatrix4x4 input);
    float x();
    float y();
    float z();

protected:
    float m_r;
    float m_theta;
    float m_phi;
    float m_vr;
    float m_vth;
    float m_vph;
    float m_rx;
    float m_ry;
    float m_rz;
    float m_rxv;
    float m_ryv;
    float m_rzv;
    float m_scale;

    float m_x;
    float m_y;
    float m_z;

};
#endif // GPARTICLE_H

