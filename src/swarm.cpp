#include "swarm.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <sailfishapp.h>

#include "math.h"

#define RADIUS 5.0
#define PI 3.1415926535
#define PI2 6.283185307

#define COUNT 1

#define TICK 17
#define VMIN -1.0
#define VMAX 1.0

Swarm::Swarm(QObject *parent) :
    GLItem()
  , m_frame(0)
  , p_numParticles(0)
  , p_depth(0)
  , p_x(0)
  , p_y(0)
  , m_thread_t(0)
  , p_pressed(false)
  , m_wind({0,0,0,0})
  , m_lastWind({0,0,0,0})
{
    m_timer.setInterval(TICK);;
    for (int n=0; n<p_numParticles; n++ ) {
        m_swarm << GParticle(RADIUS - rnd(RADIUS/2.0), rnd(PI2), rnd(PI2),  // location
                             0, 0, 1+rnd(3.0), // radial + angular velocity
                             //                             0, rnd(0.4), 1+rnd(3.0), // radial + angular velocity
                             rnd(PI2), rnd(PI2), rnd(PI2), // initial orientation
                             rnd(10), rnd(10), rnd(10), // tumble speed
                             rnd(0.3)
                             );
    }
}
float Swarm::rnd(float max) {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max);
}

void Swarm::handlePositionChanged(int x, int y)
{
    if (x != p_x) {
        p_x = x;
        emit xChanged();
    }
    if (y != p_y) {
        p_y = y;
        emit yChanged();
    }
    if (!m_lastTime.isValid()) {
        m_lastTime.start();
        return;
    }
}
void Swarm::handleOrientationChanged(int orientation) {
    m_orientationInDegrees=0;
    while ( orientation && !((1 << m_orientationInDegrees++) & orientation));
    m_orientationInDegrees-=1;
    m_orientationInDegrees = (4-m_orientationInDegrees) * 90;
    qDebug() << "Orientation set to " << m_orientationInDegrees << " by " << orientation;
}

void Swarm::setNumParticles(int n)
{
    if (n<=0) return;
    if (n == p_numParticles)
        return;
    m_swarmMutex.lock();
    if (n > p_numParticles) {
        for (int c=0; c<n-p_numParticles; c++ ) {
            m_swarm << GParticle(RADIUS - rnd(RADIUS/2.0), rnd(PI2), rnd(PI2),  // location
                                 0, 0, 1+rnd(3.0), // radial + angular velocity
                                 //                             0, rnd(0.4), 1+rnd(3.0), // radial + angular velocity
                                 rnd(PI2), rnd(PI2), rnd(PI2), // initial orientation
                                 rnd(10), rnd(10), rnd(10), // tumble speed
                                 rnd(0.3)
                                 );
        }
    } else {
        while (m_swarm.size() > n) {

            m_swarm.removeAt(rand() % (m_swarm.size()-1));
        }
    }
    m_swarmMutex.unlock();
    p_numParticles = n;
    qDebug() << "Particles now " << m_swarm.size() << " == " << n;
    emit numParticlesChanged();
}
void Swarm::setDepth(qreal d)
{
    if (d == p_depth)
        return;
    p_depth = d;
    emit depthChanged();
}
void Swarm::setX(qreal x)
{
    if (x == p_x)
        return;
    p_x = x;
    emit xChanged();
}
void Swarm::setY(qreal y)
{
    if (y == p_y)
        return;
    p_y = y;
    emit yChanged();
}

void Swarm::setPressed(bool pressed) {
    if (pressed == p_pressed)
        return;
    p_pressed = pressed;
    if (!p_pressed) {
        m_lastTime.invalidate(); // set touch velocity timer off
        m_wind.vx = 0;
        m_wind.vy = 0;
    }
    emit pressedChanged();
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
                                       SailfishApp::pathTo("swarm_vert.glsl").toLocalFile());
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                       SailfishApp::pathTo("swarm_frag.glsl").toLocalFile());

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
#define WIND_R 0.2
#define FOVY 70
#define ASPECT (540.0/960.0)
#define MAXV 1.5
    QMatrix4x4 matrix;
    // Invert the perspective matrix for mousemapping
    bool wasInverted;
    matrix.perspective(FOVY, ASPECT, 0.1, 100.0);
    matrix.rotate(m_orientationInDegrees,0,0,1);
    matrix.translate(0, 0, p_depth); // This is essentially a camera translate...
//    QMatrix4x4 inverse = matrix.inverted(&wasInverted) ;

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Because we're using VBOs this is pointer into them
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_0);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_1);

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colAttr);

    float xvp,yvp,f;
    float wind_r = 0;
    m_wind.y = 0;
    m_wind.x = 0;
    if (p_pressed) {
        // Some finger movement has started - where are we?
        // take X, Y, calculate the vpx and vpy
        xvp = (p_x-(width()/2)) / (width()/2);
        yvp = ((height()/2)-p_y) / (height()/2);

        // We've moved the world in by m_t so scale

        f = 1/tan((FOVY/2)*PI/180);

        m_wind.y = (yvp/f) * p_depth * -1;
        m_wind.x = (xvp/f) * p_depth * ASPECT * -1;
        m_wind.vx = 0;
        m_wind.vy = 0;
        // how big an area does the wind affect at depth
        wind_r = (WIND_R/f) * p_depth * -1;
    }
    if (p_pressed and m_lastTime.isValid()) {
        // Some finger movement has happened - calculate some wind.
        float timeDelta=m_lastTime.nsecsElapsed()/100000000.0;
        m_wind.vx = (m_lastWind.x - m_wind.x) / timeDelta;
        m_wind.vy = (m_lastWind.y - m_wind.y) / timeDelta;

        qDebug() << "XY (" << m_wind.x << "," << m_wind.y << ")"
                 << "Wind (" << m_wind.vx << "," << m_wind.vy << ")" << timeDelta;
        if (m_wind.vx > MAXV) m_wind.vx = MAXV;
        if (m_wind.vx < -MAXV) m_wind.vx = -MAXV;
        if (m_wind.vy > MAXV) m_wind.vy = MAXV;
        if (m_wind.vy < -MAXV) m_wind.vy = -MAXV;

        // These are the 'world points'

        QMatrix4x4 cubeM ;
        cubeM = matrix;
        cubeM.translate(m_wind.x, m_wind.y, 0);
        //        cubeM.scale(m_wind.vx, m_wind.vy, 0);
        m_program->setUniformValue(m_matrixUniform, cubeM);
        glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

        // Matrix inversion approach seems not to work. Not sure why
        //        QVector3D glSpaceXYZ = inverse.map(QVector3D(xvp, yvp, -m_t));

        //        qDebug()
        //                << "Manual Screen (" << m_x << "," << m_y << ")"
        //                << "vp(" << xvp << "," << yvp << ") "
        //                   //                << "world(" << xw << "," <<  yw << ","<< -m_t<<")"
        //                << "Matrix world("
        //                << glSpaceXYZ.x() << "," <<  glSpaceXYZ.y() << ","<< glSpaceXYZ.z()<<")";

        //        cubeM = matrix;
        //        cubeM.translate(glSpaceXYZ);

        //        m_program->setUniformValue(m_matrixUniform, cubeM);
        //        glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    }

    // Update and draw the particles.
    m_swarmMutex.lock();
    QList<GParticle>::iterator i;
    for (i = m_swarm.begin(); i != m_swarm.end(); ++i) {
        i->update(TICK/1000.0, m_wind, wind_r );
        m_program->setUniformValue(m_matrixUniform, i->matrix(matrix));
        glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    }
    m_swarmMutex.unlock();
    if (p_pressed) {
        m_lastWind.x = m_wind.x;
        m_lastWind.y = m_wind.y;
        m_lastWind.vx = m_wind.vx;
        m_lastWind.vy = m_wind.vy;
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
