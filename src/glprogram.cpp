#include "glprogram.h"

GLProgram::GLProgram(QObject *parent) :
    QOpenGLShaderProgram(parent)
{
}

GLuint GLProgram::getA(QString a) {
    if (m_attributes.contains(a))
        return m_attributes[a];
    GLuint p = this->attributeLocation(a);
    Q_ASSERT( p != -1 );
    if (p == -1) {
        return p;
    }
    m_attributes[a]=p;
    return p;
}

GLuint GLProgram::getU(QString u) {
    if (m_uniforms.contains(u))
        return m_uniforms[u];
    GLuint p = this->uniformLocation(u);
    if (p == -1) {
        qDebug() << "Uniform bad : " << u;
        Q_ASSERT( p != -1);
        return p;
    }
    m_uniforms[u]=p;
    return p;
}

