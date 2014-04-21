#ifndef GLITEM_H
#define GLITEM_H

#include <QtQuick/QQuickItem>
#include <QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>

class GLItem : public QQuickItem, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLItem();

protected:
    virtual void prep() = 0;
    virtual void render() = 0;

public slots:
    void paint();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged(QQuickWindow *win);

protected:
    bool m_donePrep;

};

#endif // GLITEM_H
