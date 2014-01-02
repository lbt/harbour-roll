#include "swarm.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QTimer>

#define DEPLOYPATH "/opt/sdk/harbour-snowglobe/usr/share/harbour-snowglobe/"

#define RADIUS 5.0
#define PI 3.1415926535
#define PI2 6.283185307

#define COUNT 400

#define TICK 17
#define VMIN -1.0
#define VMAX 1.0

Swarm::Swarm(QObject *parent) :
    GLItem()
  , m_t(0)
  , m_thread_t(0)
  , m_frame(0)
{
    m_timer.setInterval(TICK);
    m_pcount = COUNT;
    for (int n=0; n<m_pcount; n++ ) {
        m_swarm << GParticle(RADIUS - rnd(RADIUS/2.0), rnd(PI2), rnd(PI2),  // location
                             rnd(1.0), rnd(0.4), 1+rnd(3.0), // radius + angular velocity
                             rnd(PI2), rnd(PI2), rnd(PI2),
                             rnd(10), rnd(10), rnd(10),
                             rnd(0.3)
                             );
    }
}
float Swarm::rnd(float max) {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max);
}

void Swarm::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();

    //    qDebug() << "setT";
}

void Swarm::setRunning(bool running)
{
    if (running == m_timer.isActive())
        return;
    if (running)
        m_timer.start();
    else
        m_timer.stop();
    emit runningChanged();
}



void Swarm::prep()
{
    // we don't have a window to connect the timer to until now
    connect(&m_timer, SIGNAL(timeout()), this->window(), SLOT(update()) );

    // prep must add and link any shaders
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                       DEPLOYPATH "swarm_vert.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                       DEPLOYPATH "swarm_frag.glsl");

    // prep must bind any attributes
    m_program->bindAttributeLocation("vertices", 0);

    if (! m_program->link()) {
        qDebug() << "Linking failed\n" << m_program->log();
    }

    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    // and then prepare any one-time data like VBOs
    glGenBuffers(2, m_vboIds);


    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D( VMIN,  VMIN,  VMAX), QVector2D(0.0, 0.0)},  // v0
        {QVector3D( VMAX,  VMIN,  VMAX), QVector2D(0.33, 0.0)}, // v1
        {QVector3D( VMIN,  VMAX,  VMAX), QVector2D(0.0, 0.5)},  // v2
        {QVector3D( VMAX,  VMAX,  VMAX), QVector2D(0.33, 0.5)}, // v3

        // Vertex data for face 1
        {QVector3D( VMAX,  VMIN,  VMAX), QVector2D( 0.0, 0.5)}, // v4
        {QVector3D( VMAX,  VMIN,  VMIN), QVector2D(0.33, 0.5)}, // v5
        {QVector3D( VMAX,  VMAX,  VMAX), QVector2D(0.0, 1.0)},  // v6
        {QVector3D( VMAX,  VMAX,  VMIN), QVector2D(0.33, 1.0)}, // v7

        // Vertex data for face 2
        {QVector3D( VMAX,  VMIN,  VMIN), QVector2D(0.66, 0.5)}, // v8
        {QVector3D( VMIN,  VMIN,  VMIN), QVector2D(1.0, 0.5)},  // v9
        {QVector3D( VMAX,  VMAX,  VMIN), QVector2D(0.66, 1.0)}, // v10
        {QVector3D( VMIN,  VMAX,  VMIN), QVector2D(1.0, 1.0)},  // v11

        // Vertex data for face 3
        {QVector3D( VMIN,  VMIN,  VMIN), QVector2D(0.66, 0.0)}, // v12
        {QVector3D( VMIN,  VMIN,  VMAX), QVector2D(1.0, 0.0)},  // v13
        {QVector3D( VMIN,  VMAX,  VMIN), QVector2D(0.66, 0.5)}, // v14
        {QVector3D( VMIN,  VMAX,  VMAX), QVector2D(1.0, 0.5)},  // v15

        // Vertex data for face 4
        {QVector3D( VMIN,  VMIN,  VMIN), QVector2D(0.33, 0.0)}, // v16
        {QVector3D( VMAX,  VMIN,  VMIN), QVector2D(0.66, 0.0)}, // v17
        {QVector3D( VMIN,  VMIN,  VMAX), QVector2D(0.33, 0.5)}, // v18
        {QVector3D( VMAX,  VMIN,  VMAX), QVector2D(0.66, 0.5)}, // v19

        // Vertex data for face 5
        {QVector3D( VMIN,  VMAX,  VMAX), QVector2D(0.33, 0.5)}, // v20
        {QVector3D( VMAX,  VMAX,  VMAX), QVector2D(0.66, 0.5)}, // v21
        {QVector3D( VMIN,  VMAX,  VMIN), QVector2D(0.33, 1.0)}, // v22
        {QVector3D( VMAX,  VMAX,  VMIN), QVector2D(0.66, 1.0)}, // v23

    };
    GLushort indices[] = {
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23,      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 34 * sizeof(GLushort), indices, GL_STATIC_DRAW);
}

void Swarm::render()
{
    QMatrix4x4 matrix;
    matrix.perspective(60, 540.0/960.0, 0.1, 50.0);
    matrix.translate(0, 0, m_t);
    //        matrix.rotate(100.0f * m_frame / 60, 0, 1, 0);
    //        matrix.rotate(100.0f * m_frame / 400, 0, 0, 1);
    //        matrix.rotate(100.0f * m_frame / 100, 1, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Because we're using VBOs this is pointer into them
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_0);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_1);

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colAttr);

    QList<GParticle>::iterator i;
    for (i = m_swarm.begin(); i != m_swarm.end(); ++i) {
        i->update(TICK/1000.0);
        m_program->setUniformValue(m_matrixUniform, i->matrix(matrix));
        glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    }


    glDisableVertexAttribArray(m_posAttr);
    glDisableVertexAttribArray(m_colAttr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //    glClearDepthf(0.0);
    //    glClear(GL_DEPTH_BUFFER_BIT);
    //    glFlush();

}
void Swarm::sync()
{
    ++m_frame;
}
