#ifndef TRACK_H
#define TRACK_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>

class Track
{
public:
    Track();

    QString name() const { return m_name; }
    QString file() const { return m_file; }
    QString mesh() const { return m_mesh; }
    QString cameraCurve() const { return m_cameraCurve; }
    QString cameraLookAt() const { return m_cameraLookAt; }
    QVector3D start() const { return m_start; }

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

protected:
    QString m_name;
    QString m_file;
    QString m_mesh;
    QString m_cameraCurve;
    QString m_cameraLookAt;
    QVector3D m_start;

};

#endif // TRACK_H
