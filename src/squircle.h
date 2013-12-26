#ifndef SQUIRCLE_H
#define SQUIRCLE_H

// #include <QQuickItem>
#include <QtQuick/QQuickItem>
#include <QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>

class Squircle : public QQuickItem, protected QOpenGLFunctions
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Squircle();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void paint();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void renderA();
    void renderB();
    void prepC();
    void renderC();
private:
    QOpenGLShaderProgram *m_program;

    qreal m_t;
    qreal m_thread_t;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    int m_frame;
    GLuint m_vboIds[2];
};

#endif // SQUIRCLE_H
