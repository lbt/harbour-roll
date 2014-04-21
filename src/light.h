#ifndef LIGHT_H
#define LIGHT_H

class World;
#include "worlditem.h"
#include "lightmanager.h"
#include "glprogram.h"

#include <QVector3D>
#include <QDebug>

class Light : public WorldItem
{
    friend QDebug operator<<(QDebug d, Light const &l);
    Q_OBJECT
public:
    explicit Light(QString name);

    virtual void randomise() = 0;
    virtual void debugRender(QMatrix4x4 projViewMatrix) { Q_UNUSED(projViewMatrix)}
    virtual void setUniforms(GLProgram *p, int i) = 0;

    virtual void debugString(QDebug &d) const;

    virtual void addToWorld(World* world);
    virtual void removeFromWorld();

};
QDebug inline operator<<(QDebug d, Light const &l) {
    l.debugString(d);
    d.nospace() << "\n"; return d;
}
QDebug inline operator<<(QDebug d, Light* const &l) {
    l->debugString(d);
    d.nospace() << "\n"; return d;
}
//////////////////////////////////////////////////////////////////////////////

class BaseLight : public Light
{
    Q_OBJECT
public:
    explicit BaseLight(QString name) : Light(name) {}
    void setBaseLight(QVector3D Color, qreal AmbientIntensity, qreal DiffuseIntensity);

    virtual void randomise();
    virtual void setUniforms(GLProgram *p, QString varId);
    virtual void debugString(QDebug &d) const;

protected:
    QVector3D m_Color;
    qreal m_AmbientIntensity;
    qreal m_DiffuseIntensity;

};
//////////////////////////////////////////////////////////////////////////////

class PointLight : public BaseLight
{
    Q_OBJECT
public:
    explicit PointLight(QString name) : BaseLight(name) {}

    void setPointLight(QVector3D Position, qreal AConstant, qreal ALinear, qreal AExp);

    virtual void randomise();
    virtual void debugRender(QMatrix4x4 projViewMatrix);
    virtual void setUniforms(GLProgram *p, int i);
    virtual void debugString(QDebug &d) const;
protected:
    qreal m_AConstant;
    qreal m_ALinear;
    qreal m_AExp;

    static GLProgram* c_program_debug;
};

//////////////////////////////////////////////////////////////////////////////

class DirectionalLight : public BaseLight
{
    Q_OBJECT
public:
    explicit DirectionalLight(QString name) : BaseLight(name) {}
    virtual void setDirectionalLight(QVector3D Direction);

    virtual void randomise();
    virtual void setUniforms(GLProgram *p, int i);
    virtual void debugString(QDebug &d) const;
protected:
    QVector3D m_Direction;

};

#endif // LIGHT_H
