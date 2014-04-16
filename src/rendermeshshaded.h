#ifndef RENDERMESHSHADED_H
#define RENDERMESHSHADED_H
#include "renderable.h"

class RenderMeshShaded : public Renderable
{
    Q_OBJECT
public:
    explicit RenderMeshShaded(QString name, VAO *v, QObject *parent = 0);
    void setupGL();
    void render(const Shader *activeShader, QMatrix4x4 worldMatrix);

signals:

public slots:

};

#endif // RENDERMESHSHADED_H
