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
    QString mesh() const { return m_mesh; }
    QString cameraCurve() const { return m_cameraCurve; }
    QVector3D start() const { return m_start; }

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

protected:
    QString m_name;
    QString m_mesh;
    QString m_cameraCurve;
    QVector3D m_start;

};

#endif // TRACK_H
