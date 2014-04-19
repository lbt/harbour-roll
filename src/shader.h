#ifndef SHADER_H
#define SHADER_H

#include <QObject>

#include "glprogram.h"
class World;

class Shader : public QObject
{
    Q_OBJECT
    friend uint qHash(const Shader &s);
public:
    explicit Shader(QUrl v_glsl_path, QUrl f_glsl_path, World *parent = 0);
    void setupGL();
    GLProgram* getProgram() { return m_p; }

    void renderPrep();

signals:

public slots:

private:
    QUrl m_vpath;
    QUrl m_fpath;
    GLProgram* m_p;
    World* m_world;
};

#endif // SHADER_H
