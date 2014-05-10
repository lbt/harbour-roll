#include "track.h"

Track::Track()
{
}

void Track::read(const QJsonObject &json)
{
    m_name = json["name"].toString();
    m_mesh = json["mesh"].toString();
    m_cameraCurve = json["cameracurve"].toString();
    QJsonArray sA = json["start"].toArray();
    m_start = QVector3D(sA[0].toDouble(),
            sA[1].toDouble(),
            sA[2].toDouble());
}

void Track::write(QJsonObject &json) const
{
    json["name"] = m_name;
    json["mesh"] = m_mesh;
    json["cameracurve"] = m_cameraCurve;
    QJsonArray sA;
    sA[0] = m_start.x();
    sA[1] = m_start.y();
    sA[2] = m_start.z();
    json["start"] = sA;

}
