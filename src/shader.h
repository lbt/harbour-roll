#ifndef SHADER_H
#define SHADER_H

#include <QObject>

#include "glprogram.h"
#include "world.h"

class World;

class Shader : public QObject
{
    Q_OBJECT
    friend uint qHash(const Shader &s);
public:
    explicit Shader(QString v_glsl_path, QString s_glsl_path, World *parent = 0);
    void setupGL();
    GLProgram* getProgram() { return m_p; }

    void renderPrep();

signals:

public slots:

private:
    QString m_vpath;
    QString m_spath;
    GLProgram* m_p;
    World* m_world;
};

#endif // SHADER_H
