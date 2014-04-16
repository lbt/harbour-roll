#ifndef RENDERMESHTEXTURED_H
#define RENDERMESHTEXTURED_H
#include "renderable.h"
#include "texture.h"

class RenderMeshTextured : public Renderable
{
    Q_OBJECT
public:
    explicit RenderMeshTextured(QString name, VAO *v, Texture *t, QObject *parent = 0);
    virtual void setTexture(Texture *t) { m_texture = t; }
    void setupGL();
    void render(const Shader *activeShader, QMatrix4x4 worldMatrix);

signals:

public slots:
protected:
    Texture *m_texture;

};

#endif // RENDERMESHTEXTURED_H
