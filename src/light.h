#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QVector3D>
#include "lightmanager.h"
#include "glprogram.h"
#include <QDebug>

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

class Light : public QObject
{
    friend QDebug operator<<(QDebug d, Light const &l);
    Q_OBJECT
public:
    explicit Light(QString name, QObject *parent=0);

    LightManager* getLightManager() const { return m_lightManager; }
    void setLightManager(LightManager *manager) { m_lightManager = manager; }

    void update(int deltaT) { if (m_lightManager) m_lightManager->update(deltaT); }
    virtual void randomise() = 0;
    virtual void debugRender(QMatrix4x4 projViewMatrix) { Q_UNUSED(projViewMatrix)}
    virtual void setUniforms(GLProgram *p, int i) = 0;

protected:
    LightManager* m_lightManager;

};
QDebug inline operator<<(QDebug d, Light const &l) {
    d.nospace() << "Light: " << l.objectName() << "\n";
    return d;
}

class PointLight : public Light
{
    Q_OBJECT
    friend QDebug inline operator<<(QDebug d, PointLight const &l);
public:
    explicit PointLight(QString name, QObject *parent=0);

    _PointLight light() const { return m_light ; }
    void set(_PointLight light);
    void update(int deltaT);
    void randomise();
    void debugRender(QMatrix4x4 projViewMatrix);
    virtual void setUniforms(GLProgram *p, int i);
private:
    _PointLight m_light;

    static GLProgram* c_program_debug;
};
QDebug inline operator<<(QDebug d, PointLight const &l){
    d.nospace() << "Point Light: " << l.objectName() << "\n"
                << "\nColor : " << l.m_light.Base.Color
                << "\nAmbientIntensity : " << l.m_light.Base.AmbientIntensity
                << "\nDiffuse : " << l.m_light.Base.DiffuseIntensity
                << "\nPosition : " << l.m_light.Position
                << "\nAconstant : " << l.m_light.AConstant
                << "\nALinear : " << l.m_light.ALinear
                << "\nAExp : " << l.m_light.AExp;
    return d;
}

class DirectionalLight : public Light
{
    Q_OBJECT
    friend QDebug inline operator<<(QDebug d, const  DirectionalLight &l);
public:
    explicit DirectionalLight(QString name, QObject *parent=0);

    void set(_DirectionalLight light);
    _DirectionalLight light() const { return m_light ; }
    void randomise();
    virtual void setUniforms(GLProgram *p, int i);

private:
    _DirectionalLight m_light;
};
QDebug inline operator<<(QDebug d, const DirectionalLight &l){
    d.nospace() << "Directional Light: " << l.objectName() << "\n"
                << "\nColor : " << l.m_light.Base.Color
                << "\nAmbientIntensity : " << l.m_light.Base.AmbientIntensity
                << "\nDiffuse : " << l.m_light.Base.DiffuseIntensity
                << "\nDirection : " << l.m_light.Direction;
    return d;
}

#endif // LIGHT_H
