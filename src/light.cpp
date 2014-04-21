#include "light.h"
#define MAXX 2.5
#define MAXY 5.0

#include <sailfishapp.h>
#include "utils.h"
#include "world.h"

GLProgram* PointLight::c_program_debug = 0;

Light::Light(QString name, BaseMotion *motion) :
    WorldItem(name, motion)
{
    setObjectName(name);
}

void Light::addToWorld(World *world)
{
    if (inWorld()) return;
    world->lock();
    world->add(this);
    WorldItem::addToWorld(world);
    world->unlock();
}

void Light::removeFromWorld()
{
    if (! inWorld()) return;
    World* world = dynamic_cast<World*>(parent());
    world->lock();
    world->remove(this);
    WorldItem::removeFromWorld();
    world->unlock();
}

//////////////////////////////////////////////////////////////////////////////

void BaseLight::setBaseLight(QVector3D Color, qreal AmbientIntensity, qreal DiffuseIntensity)
{
    m_Color = Color;
    m_AmbientIntensity = AmbientIntensity;
    m_DiffuseIntensity = DiffuseIntensity;
}

void PointLight::setPointLight(QVector3D Position, qreal AConstant, qreal ALinear, qreal AExp)
{
    Transform t;
    t.translate(Position);
    setTransform(t);
    m_AConstant = AConstant;
    m_ALinear = ALinear;
    m_AExp = AExp;
}

void DirectionalLight::setDirectionalLight(QVector3D Direction)
{
    Transform t;
    if (Direction == QVector3D(0,0,1))
        t.lookAt(QVector3D(), Direction,QVector3D(0,-1, 0));
    else
        t.lookAt(QVector3D(), Direction,QVector3D(0, 0, 1));
    setTransform(t);
}

//////////////////////////////////////////////////////////////////////////////

void BaseLight::randomise(){
    m_Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0)).normalized();
    m_AmbientIntensity=rnd(0.5);
    m_DiffuseIntensity=1.0;
    motion()->randomise();
    qDebug() << "Randomise set to: " << this;
}

void PointLight::randomise(){
    Transform t;
    t.translate(QVector3D(rnd(MAXX)-MAXX/2, rnd(MAXY)-MAXY/2, rnd(3.0)+0.0));
    setTransform(t);
    m_AConstant = rnd(0.3);
    m_ALinear = rnd(0.5);
    m_AExp = rnd(0.15);
    BaseLight::randomise();
}


void DirectionalLight::randomise(){
    setDirectionalLight(QVector3D(rnd(10.0)-5.0,rnd(10.0)-5.0,rnd(10.0)-5.0).normalized());
    BaseLight::randomise();
}

//////////////////////////////////////////////////////////////////////////////

void Light::setUniforms(GLProgram *p, int i) {
    Q_UNUSED(p)
    Q_UNUSED(i);
    qDebug() << "Base Light has no Uniforms to set";
}

void BaseLight::setUniforms(GLProgram *p, QString ln) {
    p->setUniformValue(p->getU(ln+"Base.Color"), m_Color);
    p->setUniformValue(p->getU(ln+"Base.AmbientIntensity"), m_AmbientIntensity);
    p->setUniformValue(p->getU(ln+"Base.DiffuseIntensity"), m_DiffuseIntensity);
}

void DirectionalLight::setUniforms(GLProgram *p, int i) {
    QString ln = QString("directionalLights[%1].").arg(i);
    BaseLight::setUniforms(p, ln);
    p->setUniformValue(p->getU(ln+"Direction"), getTransform().forward());
}

void PointLight::setUniforms(GLProgram *p, int i) {
    QString ln = QString("pointLights[%1].").arg(i);
    BaseLight::setUniforms(p, ln);
    p->setUniformValue(p->getU(ln+"Position"), getTransform().at());
    p->setUniformValue(p->getU(ln+"AConstant"), m_AConstant);
    p->setUniformValue(p->getU(ln+"ALinear"), m_ALinear);
    p->setUniformValue(p->getU(ln+"AExp"), m_AExp);
}

//////////////////////////////////////////////////////////////////////////////

void PointLight::debugRender(QMatrix4x4 projViewMatrix){
    if (! c_program_debug)
        c_program_debug = new GLProgram(SailfishApp::pathTo("light_vert.glsl"), SailfishApp::pathTo("light_frag.glsl"));
    // Renders each light as a large point
    QMatrix4x4 worldMatrix; // null
    c_program_debug->bind();
    c_program_debug->setUniformValue(c_program_debug->getU("projViewMatrixU"), projViewMatrix);
    c_program_debug->setUniformValue(c_program_debug->getU("worldMatrixU"), worldMatrix);
    glEnableVertexAttribArray(c_program_debug->getA("posA"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    c_program_debug->setUniformValue(c_program_debug->getU("colU"), QVector4D(m_Color, 1.0));
    QVector3D pos = getTransform().at();
    GLfloat point[] = {pos.x(), pos.y(), pos.z(), 0.0, 0.0, 0.0 };
    glVertexAttribPointer(c_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, point);
    // it's nice to have the line drawn from the origin
    glDrawArrays(GL_POINTS, 0, 1);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(c_program_debug->getA("posA"));
    //    qDebug() << "Rendering Light at " << getTransform().at();
}

//////////////////////////////////////////////////////////////////////////////

void Light::debugString(QDebug &d) const {
    d << "Light: (" << metaObject()->className() << ") (" << objectName() << ")";
}

void BaseLight::debugString(QDebug &d) const
{ Light::debugString(d);
    d  << "\nColor : " << m_Color
       << "\nAmbientIntensity : " << m_AmbientIntensity
       << "\nDiffuse : " << m_DiffuseIntensity;
}

void PointLight::debugString(QDebug &d) const
{ BaseLight::debugString(d);
    d   << "\nPosition : " << getTransform().at()
        << "\nAconstant : " << m_AConstant
        << "\nALinear : " << m_ALinear
        << "\nAExp : " << m_AExp;
}
void DirectionalLight::debugString(QDebug &d) const
{   BaseLight::debugString(d);
    d << "\nDirection : " << getTransform().forward();
}
