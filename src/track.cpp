#include "track.h"

Track::Track()
{
}

void Track::read(const QJsonObject &json)
{
    qreal version = json["version"].toDouble();

    m_name = json["name"].toString();
    m_mesh = json["mesh"].toString();
    QJsonObject cam = json["camera"].toObject();
    m_cameraCurve = cam ["curve"].toString();
    if (cam.contains("lookat")) {
        m_cameraLookAt = cam["lookat"].toString();
    } else {
        m_cameraLookAt = "forward";
    }
    QJsonArray sA = json["start"].toArray();
    m_start = QVector3D(sA[0].toDouble(),
            sA[1].toDouble(),
            sA[2].toDouble());
}

void Track::write(QJsonObject &json) const
{
    json["version"] = 2.0;
    json["name"] = m_name;
    json["mesh"] = m_mesh;
    json["cameracurve"] = m_cameraCurve;
    QJsonArray sA;
    sA[0] = m_start.x();
    sA[1] = m_start.y();
    sA[2] = m_start.z();
    json["start"] = sA;

}
