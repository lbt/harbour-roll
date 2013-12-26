#ifndef TRIANGLEWINDOW_H
#define TRIANGLEWINDOW_H
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize();
    void render();

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
};

#endif // TRIANGLEWINDOW_H
