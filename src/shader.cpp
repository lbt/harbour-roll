#include "shader.h"
#include "world.h"

uint qHash(const Shader &s) { return qHash(s.m_p); }

Shader::Shader(QUrl v_glsl_path, QUrl f_glsl_path, World *parent) :
    QObject((QObject*)parent)
  , m_vpath(v_glsl_path)
  , m_fpath(f_glsl_path)
  , m_p(NULL)
  , m_world(parent)
{
    qDebug() << "Setup pre-GL using '" << m_vpath << "' and '" << m_fpath <<"'";
}

void Shader::setupGL()
{
    qDebug() << "Setup GL using '" << m_vpath << "' and '" << m_fpath <<"'";
    if (m_p == NULL) {
        m_p = new GLProgram(m_vpath, m_fpath);
        if (m_p && m_p->isLinked()) {
            qDebug() <<"Shader is linked";
        } else {
            qDebug() <<"Shader failed to link";
        }
    }
}

void Shader::renderPrep()
{
    if (m_p && m_p->isLinked()){
        m_p->bind();
        //        qDebug() <<"Shader is linked and bound";
    } else {
        qDebug() <<"Shader is not linked";
        return;
    }

    // Set standard camera values in the glsl
    m_p->setUniformValue(m_p->getU("projViewMatrixU"), m_world->getActiveCameraPVM());
    m_p->setUniformValue(m_p->getU("eyeWorldPosU"), m_world->getActiveCameraAt());

    int nDirLights = 0;
    int nPointLights = 0;
    for (Light* l : m_world->getLights()) {
        if (l->inherits("DirectionalLight")) {
            //            qDebug() <<"Prep light : " << *dynamic_cast<DirectionalLight*>(l);
            l->setUniforms(m_p, nDirLights++);
        } else if (l->inherits("PointLight")) {
            //            qDebug() <<"Prep light : " << *dynamic_cast<PointLight*>(l);
            l->setUniforms(m_p, nPointLights++);
        } else {
            qDebug() <<"Prep light : " << *l;
            qDebug() <<"Unknown light type: " << l->metaObject()->className();
        }
    }
}
