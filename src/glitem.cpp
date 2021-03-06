#include "glitem.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QFile>
#include <QSurfaceFormat>

#include <QDebug>

GLItem::GLItem()
    : m_donePrep(false)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}


void GLItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
        bool before = true;

        if (before)
            connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
        else
            connect(win, SIGNAL(afterRendering()), this, SLOT(paint()), Qt::DirectConnection);

        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        if (before)
            win->setClearBeforeRendering(false);
        else
            win->setClearBeforeRendering(true);
    }
}

void GLItem::paint()
{
    if (!m_donePrep) {
        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
        initializeOpenGLFunctions();
        this->prep();
        m_donePrep=true;
    }
    QRectF vpr = mapRectToScene(QRectF(0.0,0.0,width(),height()));
    glViewport( vpr.x(), (window()->height() -( vpr.y() + vpr.height())),
                vpr.width(), vpr.height());

    //    glDisable(GL_DEPTH_TEST);

    float depthrange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthrange);
    glDepthRangef(0.0, 1.0);

    float depthclear;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthclear);
    glClearDepthf(1.0);


    // http://code.woboq.org/qt5/qtdeclarative/src/quick/items/qquickwindow.cpp.html#_ZN12QQuickWindow16resetOpenGLStateEv
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_STENCIL_TEST);
//    glDisable(GL_SCISSOR_TEST);
//    glColorMask(true, true, true, true);
    glClearColor(0, 0, 0, 0);
    glDepthMask(true);
//    glDepthFunc(GL_LESS);
//    glClearDepthf(1);
//    glStencilMask(0xff);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//    glStencilFunc(GL_ALWAYS, 0, 0xff);
//    glDisable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ZERO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    // actually clear all depth information
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    this->render();

    // This is needed just in case nothing renders for a few frames
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDepthRangef(depthrange[0], depthrange[1]);
    glClearDepthf(depthclear);
}

void GLItem::cleanup()
{
    m_donePrep = false;
}
void GLItem::sync(){
}

