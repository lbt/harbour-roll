#include "shader.h"

uint qHash(const Shader &s) { qHash(s.m_p); }

Shader::Shader(QUrl v_glsl_path, QUrl f_glsl_path, World *parent) :
    QObject((QObject*)parent)
  , m_world(parent)
  , m_p(NULL)
  , m_fpath(f_glsl_path)
  , m_vpath(v_glsl_path)
{
    qDebug() << "Setup pre-GL using '" << m_vpath << "' and '" << m_fpath <<"'";
}

void Shader::setupGL()
{
    qDebug() << "Setup GL using '" << m_vpath << "' and '" << m_fpath <<"'";
    m_p = new GLProgram(m_vpath, m_fpath);
}

void Shader::renderPrep()
{

    if (m_p && m_p->isLinked()){
        m_p->bind();
        qDebug() <<"Shader is linked and bound";
    } else {
        qDebug() <<"Shader is not linked";
        return;
    }

    // Set standard camera values in the glsl
    m_p->setUniformValue(m_p->getU("projViewMatrixU"), m_world->getActiveCameraPVM());
    m_p->setUniformValue(m_p->getU("eyeWorldPosU"), m_world->getActiveCameraAt());
    QMatrix4x4 worldMatrix; // null atm - not sure if we need this
    m_p->setUniformValue(m_p->getU("worldMatrixU"), worldMatrix);

    int nDirLights = 0;
    int nPointLights = 0;
    for (Light* l : m_world->getLights()) {

        if (l->inherits("DirectionalLight")) {
            l->setUniforms(m_p, nDirLights++);
        } else if (l->inherits("PointLight")) {
            l->setUniforms(m_p, nPointLights++);
        } else {
            qDebug() <<"Unknown light type: " << l->metaObject()->className();
        }
    }
}
