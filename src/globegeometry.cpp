#include "globegeometry.h"
#include <QOpenGLFunctions>

GlobeGeometry::GlobeGeometry(QObject *parent) :
    QObject(parent),
    QOpenGLFunctions()
{
}

