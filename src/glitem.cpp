#include "glitem.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QFile>

#include <QDebug>


GLItem::GLItem()
    : m_program(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    qDebug() << "Created";
}


void GLItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection

        connect(win, SIGNAL(afterRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}

void GLItem::paint()
{
    if (!m_program) {
        qDebug() << "create program";
        m_program = new QOpenGLShaderProgram();

        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
        //window()->setClearBeforeRendering(false);
        window()->setClearBeforeRendering(true);
        initializeOpenGLFunctions();
        this->prep();
    }
    QRectF vpr = mapRectToScene(QRectF(0.0,0.0,width(),height()));
    //    qDebug()<< "Item width "<< width()<< "height "<< height();
    //    qDebug()<< "transform  x "<< vpr.x()<< "y "<< vpr.y()<< "width "<< vpr.width()<< "height "<< vpr.height();
    //    qDebug()<< "Window width "<< window()->width()<< "height "<< window()->height();
    glViewport( vpr.x(), (window()->height() -( vpr.y() + vpr.height())),
                vpr.width(), vpr.height());

    glDisable(GL_DEPTH_TEST);
    //    glEnable(GL_DEPTH_TEST); // I still think this should be set after clearing the depth buffer

    glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Bind the program and render
    m_program->bind();
    this->render();
    m_program->release();
}

void GLItem::cleanup()
{
    if (m_program) {
        qDebug() << "cleanup program";
        delete m_program;
        m_program = 0;
    }
}
void GLItem::sync(){
}

