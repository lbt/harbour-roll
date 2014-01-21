#include "swarm.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QtGui/QOpenGLContext>
#include <QImage>
#include <QVector3D>

#include <sailfishapp.h>

#include "math.h"

#include <QDebug>

#define RADIUS 5.0
#define PI 3.1415926535
#define PI2 6.283185307

#define COUNT 1

#define TICK 17
#define VMIN -1.0
#define VMAX 1.0

#define WIND_R 0.1
#define FOVY 70
#define ASPECT (540.0/960.0)
#define MAXV 1.5

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
  , m_cameraPos({0,0,0.2})
  , m_cameraRot({0,0,0})
{
    Q_UNUSED(parent)
    m_timer.setInterval(TICK);;
    for (int n=0; n<p_numParticles; n++ ) {
        m_swarm << GParticle2(RADIUS - rnd(RADIUS/2.0), rnd(PI2), rnd(PI2),  // location
                              0, 0, 1+rnd(3.0), // radial + angular velocity
                              //                             0, rnd(0.4), 1+rnd(3.0), // radial + angular velocity
                              rnd(PI2), rnd(PI2), rnd(PI2), // initial orientation
                              rnd(10), rnd(10), rnd(10), // tumble speed
                              rnd(0.3)
                              );
    }
    m_sensor.start();

    for (int i=0; i<3; i++) {
        m_dLights[i].Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0));
        m_dLights[i].Base.Color = QVector3D(0,0,0);
        m_dLights[i].Base.AmbientIntensity=0.0;
        m_dLights[i].Base.DiffuseIntensity=0.5;
        m_dLights[i].Direction = QVector3D(rnd(10.0)-5.0,rnd(10.0)-5.0,rnd(10.0)-5.0).normalized();
    }


    for (int i=0; i<3; i++) {
        m_pLights[i].Base.Color = QVector3D(rnd(1.0),rnd(1.0),rnd(1.0));
        qDebug() << "Setting plights[" << i << "] " << m_pLights[i].Base.Color ;
        //m_pLights[i].Base.Color = QVector3D(0,0,0);
        m_pLights[i].Base.AmbientIntensity=0.0;
        m_pLights[i].Base.DiffuseIntensity=1.0;
        m_pLights[i].Position = QVector3D(rnd(10.0)-5.0,rnd(10.0)-5.0,rnd(10.0)-5.0);
        m_pLights[i].AConstant = 0.1;
        m_pLights[i].ALinear = 0.5;
        m_pLights[i].AExp = 0.1;
    }
//    m_pLights[0].Base.Color = QVector3D(1,0,0);
//    m_pLights[1].Base.Color = QVector3D(0,1,0);
//    m_pLights[2].Base.Color = QVector3D(0,0,1);
//    m_pLights[0].Position = QVector3D(5, 5, 5);
//    m_pLights[1].Position = QVector3D(0, 0, 0);
//    m_pLights[2].Position = QVector3D(0, 0, -5);
    //    m_pLights[0].Base.Color = QVector3D(0.5+rnd(0.5), 0.5+rnd(0.5), 0.5+rnd(0.5));

}
float Swarm::rnd(float max) {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max);
}

void Swarm::handleOrientationChanged(int orientation) {
    m_orientationInDegrees=0;
    while ( orientation && !((1 << m_orientationInDegrees++) & orientation));
    m_orientationInDegrees-=1;
    m_orientationInDegrees = (4-m_orientationInDegrees) * 90;
    qDebug() << "Orientation set to " << m_orientationInDegrees << " by " << orientation;
}
void Swarm::setOrientationInDegrees(int d)
{
    if (d == p_orientationInDegrees)
        return;
    p_orientationInDegrees = d;
    m_orientationInDegrees = p_orientationInDegrees;
    emit orientationInDegreesChanged();
}

void Swarm::setNumParticles(int n)
{
    if (n<=0) return;
    if (n == p_numParticles)
        return;
    m_swarmMutex.lock();
    if (n > p_numParticles) {
        for (int c=0; c<n-p_numParticles; c++ ) {
            m_swarm << GParticle2(RADIUS - rnd(RADIUS/2.0), rnd(PI2), rnd(PI2),  // location
                                  0, 0, 1+rnd(3.0), // radial + angular velocity
                                  //                             0, rnd(0.4), 1+rnd(3.0), // radial + angular velocity
                                  rnd(PI2), rnd(PI2), rnd(PI2), // initial orientation
                                  rnd(2), rnd(2), rnd(2), // tumble speed
                                  rnd(0.5)+0.5 // scale
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
    emit xChanged(x);
}
void Swarm::setY(qreal y)
{
    if (y == p_y)
        return;
    p_y = y;
    emit yChanged(y);
}
void Swarm::setZ(qreal z)
{
    if (z == p_z)
        return;
    p_z = z;
    emit zChanged(z);
}
void Swarm::setXYZ(QVector3D v) {
    setX(v.x());
    setY(v.y());
    setZ(v.z());
}

void Swarm::useXYZ(QString use) {
    m_use = use;
    if (m_use == "light1 col") {
        setXYZ(m_dLights[0].Base.Color);
    } else if (m_use == "light1 dir") {
        setXYZ((m_dLights[0].Direction+QVector3D(1,1,1))*QVector3D(0.5,0.5,0.5));
    } else if (m_use == "light2 col") {
        setXYZ(m_dLights[1].Base.Color);
    } else if (m_use == "light2 dir") {
        setXYZ((m_dLights[1].Direction+QVector3D(1,1,1))*QVector3D(0.5,0.5,0.5));
    }
}

void Swarm::handleUse() {
    if (m_use == "light1 col") {
        m_dLights[0].Base.Color = QVector3D(p_x, p_y, p_z);
    } else if (m_use == "light1 dir") {
        m_dLights[1].Direction = QVector3D((p_x - 0.5)*2,
                                           (p_y - 0.5)*2,
                                           (p_z - 0.5)*2);
    } else if (m_use == "light2 col") {
        m_dLights[1].Base.Color = QVector3D(p_x, p_y, p_z);
    } else if (m_use == "light2 dir") {
        m_dLights[1].Direction = QVector3D((p_x - 0.5)*2,
                                           (p_y - 0.5)*2,
                                           (p_z - 0.5)*2);
    }
}

void Swarm::handlePositionChanged(int x, int y)
{
    m_lastx = x;
    m_lasty = y;
    m_XYdeltaTime = m_lastTime.restart();
    if (x != p_x) {
        p_x = x;
        emit xChanged(p_x);
    }
    if (y != p_y) {
        p_y = y;
        emit yChanged(p_y);
    }
}

void Swarm::handlePressed(int x, int y) {
    p_pressed = true;
    m_lastTime.start(); // set touch velocity timer on when touched
    p_x = x;
    p_y = y;
    m_XYdeltaTime = 0;
}
void Swarm::handleReleased(int x, int y) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    p_pressed = false;
    m_lastTime.invalidate(); // set touch velocity timer off when released
    m_wind.vx = 0;
    m_wind.vy = 0;
    m_XYdeltaTime = 0;
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
    m_program_particle = new GLProgram();
    m_program_line = new GLProgram();
    qDebug() << "created programs";

    // prep must add and link any shaders
    m_program_particle->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                SailfishApp::pathTo("swarm_vert.glsl").toLocalFile());
    m_program_particle->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                SailfishApp::pathTo("swarm_frag.glsl").toLocalFile());
    m_program_line->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                            SailfishApp::pathTo("swarm_vert.glsl").toLocalFile());
    m_program_line->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                            SailfishApp::pathTo("swarm_line_frag.glsl").toLocalFile());

    // prep must bind any attributes
    //    m_program_particle->bindAttributeLocation("vertices", 0);


    if (! m_program_particle->link()) {
        qDebug() << "Linking failed\n" << m_program_particle->log();
        QList<QOpenGLShader *>::iterator i;
        for (i = m_program_particle->shaders().begin(); i != m_program_particle->shaders().end(); ++i) {
            if ((*i)->isCompiled())
                qDebug() << "Shader compile log: \n" << (*i)->log();
        }

    }
    if (! m_program_line->link()) {
        qDebug() << "Linking failed\n" << m_program_line->log();
    }

    // and then prepare any one-time data like VBOs
    glGenBuffers(4, m_vboIds);

    VertexData vertices[] = {
    #include "cube_vertices_vec_tex_norm.data"
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
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 34 * sizeof(GLushort), indices, GL_STATIC_DRAW);

#define AXIS_LEN 1
    QVector3D axes[] = {
        QVector3D(-AXIS_LEN, 0, 0), QVector3D(AXIS_LEN, 0, 0),
        QVector3D(0, -AXIS_LEN, 0), QVector3D(0, AXIS_LEN, 0),
        QVector3D(0, 0, -AXIS_LEN), QVector3D(0, 0, AXIS_LEN),
    };
    GLushort axesorder[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5};

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(QVector3D), axes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10 * sizeof(GLushort), axesorder, GL_STATIC_DRAW);

    // Load cube.png image
    glEnable(GL_TEXTURE_2D);
    m_texture = window()->
            createTextureFromImage(QImage(SailfishApp::pathTo("cube.png")
                                          .toLocalFile()));
    if (m_texture->isAtlasTexture()) {
        qDebug() << "Removed texture from Atlas";
        m_texture = m_texture->removedFromAtlas();
    }

    m_texture->setFiltering(QSGTexture::Linear);
    m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    m_texture->setVerticalWrapMode(QSGTexture::Repeat);

    // Set nearest filtering mode for texture minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set bilinear filtering mode for texture magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Swarm::handleTouchAsWind(GLProgram *p) {
    float xvp,yvp,f;
    m_wind.r = 0;
    m_wind.y = 0;
    m_wind.x = 0;
    if (p_pressed) {
        // Some finger movement has started - where are we?
        // take X, Y, calculate the vpx and vpy
        // This is broken for non-portrait orientation
        xvp = (p_x-(width()/2)) / (width()/2);
        yvp = ((height()/2)-p_y) / (height()/2);

        // We've moved the world in by m_t so scale

        f = 1/tan((FOVY/2)*PI/180);

        m_wind.y = (yvp/f) * p_depth * -1;
        m_wind.x = (xvp/f) * p_depth * ASPECT * -1;
        m_wind.vx = 0;
        m_wind.vy = 0;
        // how big an area does the wind affect at depth
        m_wind.r = (WIND_R/f) * p_depth * -1;
    }
    if (p_pressed and m_XYdeltaTime != 0) {
        // Some finger movement has happened - calculate some wind.
        float timeDelta=m_XYdeltaTime/100.0;
        m_wind.vx = (m_lastWind.x - m_wind.x) / timeDelta;
        m_wind.vy = (m_lastWind.y - m_wind.y) / timeDelta;

        qDebug() << "XY (" << m_wind.x << "," << m_wind.y << ")"
                 << "Wind (" << m_wind.vx << "," << m_wind.vy << ")" << timeDelta;
        if (m_wind.vx > MAXV) m_wind.vx = MAXV;
        if (m_wind.vx < -MAXV) m_wind.vx = -MAXV;
        if (m_wind.vy > MAXV) m_wind.vy = MAXV;
        if (m_wind.vy < -MAXV) m_wind.vy = -MAXV;

        // These are the 'world points'

        QMatrix4x4 cubeM;
        cubeM.translate(m_wind.x, m_wind.y, 0);
        //        cubeM.scale(m_wind.vx, m_wind.vy, 0);
        p->setUniformValue(p->getU("worldMatrixU"), cubeM);
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
}

void Swarm::handleTouchAsRotation(GLProgram *p){
    if (p_pressed) {
        m_rotmanager.touch(p_x, p_y);
    } else {
        m_rotmanager.release();
    }
}

void Swarm::render()
{

    handleUse();

    GLProgram *p = m_program_particle;
    p->bind();
    QMatrix4x4 projMatrix;
    projMatrix.perspective(FOVY, ASPECT, 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed
    p->setUniformValue(p->getU("projMatrixU"), projMatrix);

    handleTouchAsRotation(p);

    QMatrix4x4 viewMatrix;
    viewMatrix = m_rotmanager.transform(viewMatrix);
    //    viewMatrix.rotate(m_orientationInDegrees,0,0,1); // handle device rotation

    //    viewMatrix.translate(0.0, -0.0, p_depth); // This is essentially a camera translate...
    p->setUniformValue(p->getU("viewMatrixU"), viewMatrix);

    // Bind the texture
    m_texture->bind();
    // Use texture unit 0 which contains cube.png
    p->setUniformValue(p->getU("textureU"), 0);


    // Setup Model
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Because we're using VBOs for vertex, tex and normals this is pointer into them
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_0);
    glVertexAttribPointer(p->getA("texA"), 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_1);
    glVertexAttribPointer(p->getA("normalA"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_2);

    glEnableVertexAttribArray(p->getA("posA"));
    glEnableVertexAttribArray(p->getA("texA"));
    glEnableVertexAttribArray(p->getA("normalA"));

    // handleTouchAsWind(p);

    // Some accel based colour
    QAccelerometerReading *reading = m_sensor.reading();

    for (unsigned int i = 0 ; i < 2 ; i++) {
        QString pln("directionalLights[%1].");
        p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_dLights[i].Base.Color);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_dLights[i].Base.AmbientIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_dLights[i].Base.DiffuseIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Direction"), m_dLights[i].Direction);
    }

    for (unsigned int i = 0 ; i < 3 ; i++) {
        QString pln("pointLights[%1].");
        p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_pLights[i].Base.Color);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_pLights[i].Base.AmbientIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_pLights[i].Base.DiffuseIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Position"), m_pLights[i].Position);
        //qDebug() << "Setting " << pln.arg(i)+"Position" << "] " << m_pLights[i].Position << " in " << p->getU(pln.arg(i)+"Position");
        p->setUniformValue(p->getU(pln.arg(i)+"AConstant"), m_pLights[i].AConstant);
        p->setUniformValue(p->getU(pln.arg(i)+"ALinear"), m_pLights[i].ALinear);
        p->setUniformValue(p->getU(pln.arg(i)+"AExp"), m_pLights[i].AExp);
    }

    p->setUniformValue(p->getU("matSpecularIntensityU"), 1.0f);
    p->setUniformValue(p->getU("specularPowerU"), 32.0f);
    p->setUniformValue(p->getU("eyeWorldPosU"), m_rotmanager.at());

    // Update and draw the particles.
    GParticle2::Accel a= {reading->x(), reading->y(), reading->z()};
    //    qDebug() << "Accel a(" << a.x << "," << a.y<<"," << a.z << ")";
    m_swarmMutex.lock();
    QList<GParticle2>::iterator i;
    int modelN=0;
    for (i = m_swarm.begin(); i != m_swarm.end(); ++i,++modelN) {
        //        i->update(TICK/1000.0, m_wind,  a );
        i->update(TICK/1000.0, m_wind, {0,0,0} );
        p->setUniformValue(p->getU("worldMatrixU"), i->worldMatrix());
        glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
    }
    m_swarmMutex.unlock();
    if (p_pressed) {
        m_lastWind.x = m_wind.x;
        m_lastWind.y = m_wind.y;
        m_lastWind.vx = m_wind.vx;
        m_lastWind.vy = m_wind.vy;
    }

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));


    // Now draw some axes
    p = m_program_line;
    p->bind();
    p->setUniformValue(p->getU("colU"), QVector4D(1, 1, 1, 1));
    p->setUniformValue(p->getU("projMatrixU"), projMatrix);
    p->setUniformValue(p->getU("viewMatrixU"), viewMatrix);

    // This bit draws lines by assigning them to a VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[3]);
    // Because we're using VBOs for vertex, tex and normals this is pointer into them
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), 0);
    glEnableVertexAttribArray(p->getA("posA"));

    QMatrix4x4 originM;
    p->setUniformValue(p->getU("worldMatrixU"), originM);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(1);
    //    glDrawElements(GL_LINE_STRIP, 10, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_LINES, 0, 6);


    // This bit draws a line directly from a Qt array
    glBindBuffer(GL_ARRAY_BUFFER, 0); // use CPU-side data
    glEnableVertexAttribArray(p->getA("posA"));

    QVector3D diag[] = {
        QVector3D(0, 0, 0), QVector3D(0, 0, 0)
    };

    // Set a colour for the shader
    //    p->setUniformValue(p->getU("colU"), QVector4D(aLight.Color, 1));
    //    diag[0]= aLight.Direction;
    //    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
    //    glDrawArrays(GL_LINES, 0, 2);

    //    p->setUniformValue(p->getU("colU"), QVector4D(bLight.Color, 1));
    //    diag[0]= bLight.Direction;
    //    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
    //    glDrawArrays(GL_LINES, 0, 2);

    glLineWidth(3);
    for (unsigned int i = 0 ; i < 2 ; i++) {
        p->setUniformValue(p->getU("colU"), QVector4D(m_dLights[i].Base.Color, 1));
        diag[0]= m_dLights[i].Direction;
        glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
        glDrawArrays(GL_LINES, 0, 2);
    }
    glLineWidth(5);
    for (unsigned int i = 0 ; i < 3 ; i++) {
        p->setUniformValue(p->getU("colU"), QVector4D(m_pLights[i].Base.Color, 1));
        diag[0]= m_pLights[i].Position;
        glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
        glDrawArrays(GL_LINES, 0, 2);
    }

    glLineWidth(5);
    p->setUniformValue(p->getU("colU"), QVector4D(1,0,0, 1));
    diag[0]= m_rotmanager.at();
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
    glDrawArrays(GL_LINES, 0, 2);
    glLineWidth(3);
    p->setUniformValue(p->getU("colU"), QVector4D(0,0,1, 1));
    diag[0]= m_rotmanager.at();
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, diag);
    glDrawArrays(GL_LINES, 0, 2);


    p->release();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Multiple draw types
    // 1. Static World in a static VBO. Rare changes
    // 2. Many moving objects in a dynamic VBO. Changes every frame
    // 3. Ad-hoc drawing

}
void Swarm::sync()
{
    ++m_frame;
}
