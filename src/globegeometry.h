#ifndef GLOBEGEOMETRY_H
#define GLOBEGEOMETRY_H

#include <QOpenGLFunctions>

class GlobeGeometry : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GlobeGeometry(QObject *parent = 0);

signals:

public slots:

};

#endif // GLOBEGEOMETRY_H
