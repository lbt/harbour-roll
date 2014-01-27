#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>
#include "lightmanager.h"

struct _BaseLight
{
    QVector3D Color;
    qreal AmbientIntensity;
    qreal DiffuseIntensity;
};
struct _DirectionalLight
{
    _BaseLight Base;
    QVector3D Direction;
};

struct _PointLight
{
    _BaseLight Base;
    QVector3D Position;
    qreal AConstant;
    qreal ALinear;
    qreal AExp;
};

class PointLight
{

public:
    PointLight();

    _PointLight light() const { return m_light ; }
    void set(_PointLight light);
    void randomise();
    LightManager lightManager;
    void update(int deltaT);

private:
    _PointLight m_light;

};

class DirectionalLight
{

public:
    DirectionalLight();

    void set(_DirectionalLight light);
    _DirectionalLight light() const { return m_light ; }
    void randomise();
private:
    _DirectionalLight m_light;
    LightManager m_mgr;
};

#endif // LIGHT_H
