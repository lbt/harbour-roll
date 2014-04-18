#include "lightmanager.h"
#include <math.h>

#define RADIUS 1.0
#define DECEL 0.999
#define MAXV 10.0


LightManager::LightManager(QObject *parent) :
    QObject(parent)
  , m_transform()
{}

LightManager::LightManager(Transform t, QObject *parent) :
    QObject(parent)
  , m_transform(t)
{
}
