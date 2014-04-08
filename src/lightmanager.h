#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <QMatrix4x4>

class LightManager
{
public:
    explicit LightManager();
    LightManager(qreal radius, qreal theta, qreal phi, qreal vr, qreal vth, qreal vph, qreal rx, qreal ry, qreal rz, qreal rxv, qreal ryv, qreal rzv, qreal scale);

    void update(int ms);
    void active(bool state) { m_active = state; }
    QMatrix4x4 matrix(QMatrix4x4 input);
    QVector3D pos();


    QVector3D scale() const;
    void setScale(const QVector3D &scale);

public slots:
    void randomise();
private:
    qreal m_r;    // location
    qreal m_theta;  // location
    qreal m_phi;      // location
    qreal m_vr;        // radial location velocity
    qreal m_vth;      // theta location velocity
    qreal m_vph;      // phi location velocity
    qreal m_rx;        // orientation x
    qreal m_ry;        // orientation y
    qreal m_rz;        // orientation z
    qreal m_rxv;      // orientation x velocity
    qreal m_ryv;      // orientation y velocity
    qreal m_rzv;      // orientation z velocity
    QVector3D m_scale;  // scale (xyz)};

    qreal m_x;  // calculated x
    qreal m_y;  // calculated y
    qreal m_z;  // calculated z

    bool m_active;
};

#endif // LIGHTMANAGER_H
