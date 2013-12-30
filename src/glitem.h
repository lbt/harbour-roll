#ifndef GLITEM_H
#define GLITEM_H

// #include <QQuickItem>
#include <QtQuick/QQuickItem>
#include <QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>

class GLItem : public QQuickItem, protected QOpenGLFunctions
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    GLItem();

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
    void prep();
    void render();
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

#endif // GLITEM_H
