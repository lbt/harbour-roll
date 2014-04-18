#include "light.h"
#define MAXX 2.5
#define MAXY 5.0

#include <sailfishapp.h>
#include "utils.h"

GLProgram* PointLight::c_program_debug = 0;

Light::Light(QString name, QObject *parent) :
    QObject(parent)
  , m_lightManager(NULL)
{
    setObjectName(name);
}
void Light::setUniforms(GLProgram *p, int i) {
    Q_UNUSED(p)
    Q_UNUSED(i);
    qDebug() << "Base Light has no Uniforms to set";
}

PointLight::PointLight(QString name, QObject *parent) :
    Light(name, parent)
{}

void PointLight::update(int deltaT)
{
    if (m_lightManager) {
        m_lightManager->update(deltaT);
        m_light.Position= m_lightManager->getTransform().at();
    }
}

void PointLight::set(_PointLight light)
{
    m_light = light;
}

void PointLight::randomise(){
    m_light.Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0)).normalized();
    m_light.Base.AmbientIntensity=rnd(0.5);
    m_light.Base.DiffuseIntensity=1.0;
    m_light.Position = QVector3D(rnd(MAXX)-MAXX/2, rnd(MAXY)-MAXY/2, rnd(3.0)+0.0);
    m_light.AConstant = rnd(0.3);
    m_light.ALinear = rnd(0.5);
    m_light.AExp = rnd(0.15);
    if (m_lightManager) m_lightManager->randomise();
    update(0);
    qDebug() << "Randomise set to: " << *this;
}

DirectionalLight::DirectionalLight(QString name, QObject *parent) :
    Light(name, parent)
{}

void DirectionalLight::set(_DirectionalLight light)
{
    m_light = light;
}

void DirectionalLight::randomise(){
    m_light.Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0)).normalized();
    m_light.Base.AmbientIntensity=rnd(0.1);
    m_light.Base.DiffuseIntensity=rnd(0.5);
    m_light.Direction = QVector3D(rnd(10.0)-5.0,rnd(10.0)-5.0,rnd(10.0)-5.0).normalized();
}
void DirectionalLight::setUniforms(GLProgram *p, int i) {
    QString pln("directionalLights[%1].");
    p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_light.Base.Color);
    p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_light.Base.AmbientIntensity);
    p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_light.Base.DiffuseIntensity);
    p->setUniformValue(p->getU(pln.arg(i)+"Direction"), m_light.Direction);
}

void PointLight::setUniforms(GLProgram *p, int i) {
    QString pln("pointLights[%1].");
    p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_light.Base.Color);
    p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_light.Base.AmbientIntensity);
    p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_light.Base.DiffuseIntensity);
    p->setUniformValue(p->getU(pln.arg(i)+"Position"), m_light.Position);
    p->setUniformValue(p->getU(pln.arg(i)+"AConstant"), m_light.AConstant);
    p->setUniformValue(p->getU(pln.arg(i)+"ALinear"), m_light.ALinear);
    p->setUniformValue(p->getU(pln.arg(i)+"AExp"), m_light.AExp);
}

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

    c_program_debug->setUniformValue(c_program_debug->getU("colU"), QVector4D(m_light.Base.Color, 1.0));
    GLfloat point[] = {m_light.Position.x(), m_light.Position.y(), m_light.Position.z(), 0.0, 0.0, 0.0 };
    glVertexAttribPointer(c_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, point);
    // it's nice to have the line drawn from the origin
    glDrawArrays(GL_POINTS, 0, 1);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(c_program_debug->getA("posA"));
    //    qDebug() << "Rendering Light at " << m_light.Position;
}
