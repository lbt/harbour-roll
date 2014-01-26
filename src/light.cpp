#include "light.h"
#define MAXX 2.5
#define MAXY 5.0

namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

void PointLight::update(int deltaT)
{
    lightManager.update(deltaT);
    m_light.Position= lightManager.pos();
}

PointLight::PointLight()
{
}

void PointLight::randomise(){
    m_light.Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0));
    m_light.Base.AmbientIntensity=rnd(0.5);
    m_light.Base.DiffuseIntensity=1.0;
    m_light.Position = QVector3D(rnd(MAXX)-MAXX/2,rnd(MAXY)-MAXY/2,rnd(3.0)+0.0);
    m_light.AConstant = rnd(0.3);
    m_light.ALinear = rnd(0.5);
    m_light.AExp = rnd(0.15);
    lightManager.randomise();
    update(0);
}

DirectionalLight::DirectionalLight()
{

}
void DirectionalLight::randomise(){
    m_light.Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0));
    m_light.Base.AmbientIntensity=0.0;
    m_light.Base.DiffuseIntensity=0.1;
    m_light.Direction = QVector3D(rnd(10.0)-5.0,rnd(10.0)-5.0,rnd(10.0)-5.0).normalized();
}
