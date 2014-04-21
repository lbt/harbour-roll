#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QObject>

class Shader;      // Mutual link
#include "glprogram.h"
#include "vao.h"

class Renderable : public QObject
{
    Q_OBJECT
public:
    explicit Renderable(QString name, VAO* v, QObject *parent = 0);
    virtual void setShader(Shader* p) { m_shader = p; }
    virtual Shader* getShader() const { return m_shader; }
    virtual void setTranslucent(bool translucent) { m_opaque = ! translucent; }
    virtual bool isTranslucent() { return !m_opaque ; }
    virtual bool isOpaque() const { return m_opaque ; }

    virtual void setupGL() = 0;
    virtual void render(const Shader *activeShader, QMatrix4x4 worldMatrix) = 0;

signals:

public slots:

protected:
    Shader* m_shader;
    VAO *m_vao;
    GLuint m_vboIds[2];
    bool m_opaque;
};

#endif // RENDERABLE_H
