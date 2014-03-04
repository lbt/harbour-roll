#include "glprogram.h"

GLProgram::GLProgram(QUrl vpath, QUrl fpath, QObject *parent) :
    QOpenGLShaderProgram(parent)
{
    this->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             vpath.toLocalFile());
    this->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             fpath.toLocalFile());
    if (! this->link()) {
        QList<QOpenGLShader *>::iterator i;
        for (i = this->shaders().begin(); i != this->shaders().end(); ++i) {
            if ((*i)->isCompiled())
                qDebug() << "Shader compile log: \n" << (*i)->log();
        }
    }
}

GLuint GLProgram::getA(QString a) {
    if (m_attributes.contains(a))
        return m_attributes[a];
    GLuint p = this->attributeLocation(a);
//    Q_ASSERT( p != -1 );
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
//        qDebug() << "Uniform bad : " << u;
//        Q_ASSERT( p != -1);
        return p;
    }
    m_uniforms[u]=p;
    return p;
}

