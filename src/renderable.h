#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QMap>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

// When the 5.2 Qt hits use this:
// #include <QOpenGLTexture>
#include <QSGTexture>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
class Shader;      // Mutual link
#include "glprogram.h"
#include "texture.h"
#include "vao.h"

class Renderable : public QObject
{
    Q_OBJECT
public:
    explicit Renderable(QString name, VAO* v, Texture *t = 0, QObject *parent = 0);
    void setTexture(Texture *t) { m_texture = t; }
    void setShader(Shader* p) { m_shader = p; }
    Shader* getShader() const { return m_shader; }
    void setupGL();
    void render(const Shader *activeShader);

signals:

public slots:

private:
    Shader* m_shader;
    VAO *m_vao;
    Texture *m_texture;
    GLuint m_vboIds[2];
};

#endif // RENDERABLE_H