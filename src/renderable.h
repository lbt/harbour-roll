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
#include "vao.h"

class Renderable : public QObject
{
    Q_OBJECT
public:
    explicit Renderable(VAO* v, QSGTexture* t = 0, QObject *parent = 0);
    void setTexture(QSGTexture *t) { m_texture = t; }
    void setShader(Shader* p);
    Shader* getShader() const { return m_shader; }
    void setupGL();
    void render(const Shader *activeShader);

signals:

public slots:

private:
    Shader* m_shader;
    QString m_name;
    VAO *m_vao;
    QSGTexture *m_texture;
    GLuint m_vboIds[3];
    GLuint m_texIds[1];
    void * m_texCoords;
    void * m_meshVertices;
};

#endif // RENDERABLE_H
