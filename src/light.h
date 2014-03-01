#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>
#include "lightmanager.h"
#include "glprogram.h"

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

class Light
{

public:
    Light();

    LightManager lightManager;
    void update(int deltaT) { }
    virtual void randomise() = 0;
    void debugRender(QMatrix4x4 projViewMatrix) {}
};

class PointLight : public Light
{

public:
    PointLight();

    _PointLight light() const { return m_light ; }
    void set(_PointLight light);
    void update(int deltaT);
    void randomise();
    void debugRender(QMatrix4x4 projViewMatrix);
private:
    _PointLight m_light;

    static GLProgram* c_program_debug;
};

class DirectionalLight : public Light
{

public:
    DirectionalLight();

    void set(_DirectionalLight light);
    _DirectionalLight light() const { return m_light ; }
    void randomise();

private:
    _DirectionalLight m_light;
};

#endif // LIGHT_H
