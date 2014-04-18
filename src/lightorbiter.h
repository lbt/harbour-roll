#ifndef LIGHTORBITER_H
#define LIGHTORBITER_H
#include "lightmanager.h"
#include <QMatrix4x4>
#include <QVector3D>

class LightOrbiter : public LightManager
{
    Q_OBJECT
public:
    explicit LightOrbiter(QObject *parent = 0);
    explicit LightOrbiter(qreal radius, qreal theta, qreal phi, qreal vr, qreal vth, qreal vph, qreal rx, qreal ry, qreal rz, qreal rxv, qreal ryv, qreal rzv, QVector3D scale, QObject *parent = 0);
    QVector3D getScale() const { return m_scale; }
    void setScale(const QVector3D &scale) { m_scale = scale; }
    void active(bool state) { m_active = state; }

    QMatrix4x4 matrix(QMatrix4x4 input); // Unused?

signals:

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

#endif // LIGHTORBITER_H
