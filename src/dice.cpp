#include "dice.h"
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
#define FOVY 50
#define ASPECT (540.0/960.0)
#define MAXV 1.5

#define MAXX 2.5
#define MAXY 5.0

namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

Dice::Dice(QObject *parent) :
    GLItem()
  , m_frame(0)
  , p_x(0)
  , p_y(0)
  , m_thread_t(0)
  , p_pressed(false)
  , m_cameraPos({0,0,0.2})
  , m_cameraRot({0,0,0})
  , m_zoomAndSpin(false)
  , m_numDice(6)
{
    Q_UNUSED(parent)
    m_timer.setInterval(TICK);;
    m_sensor.start();

    for (int i=0; i<3; i++) { m_dLights[i].randomise(); }
    for (int i=0; i<3; i++) {
        m_pLights[i].lightManager.setScale(QVector3D(4.0, 5.0, 3.0));
        m_pLights[i].randomise();
    }
}

void Dice::setX(qreal x)
{
    if (x == p_x)
        return;
    p_x = x;
    emit xChanged(x);
}
void Dice::setY(qreal y)
{
    if (y == p_y)
        return;
    p_y = y;
    emit yChanged(y);
}
void Dice::setZ(qreal z)
{
    if (z == p_z)
        return;
    p_z = z;
    emit zChanged(z);
}
void Dice::setXYZ(QVector3D v) {
    setX(v.x());
    setY(v.y());
    setZ(v.z());
}

void Dice::useXYZ(QString use) {
    m_use = use;
}

void Dice::randomiseLights()
{
    for (int i=0; i<3; i++) {
        m_dLights[i].randomise();
    }
    for (int i=0; i<3; i++) {
        m_pLights[i].randomise();
    }
}

void Dice::zoomAndSpin(bool state)
{
    m_zoomAndSpin = state;
    if (!state)
        m_cammanager.reset();
}

void Dice::setNumDice(int arg)
{
    if (m_numDice != arg) {
        m_numDice = arg;
        bullet.setNumCubes(m_numDice);
        emit numDiceChanged(arg);
    }
}

void Dice::fancyLights(bool state)
{
    m_fancyLights = state;
}

void Dice::handleTouchAsRotation(){
    if (p_pressed) {
        m_cammanager.touch(p_x, p_y);
    } else {
        m_cammanager.release();
    }
}

void Dice::handleUse() {
}

void Dice::handlePositionChanged(int x, int y)
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

void Dice::handlePressed(int x, int y) {
    p_pressed = true;
    m_lastTime.start(); // set touch velocity timer on when touched
    p_x = x;
    p_y = y;
    m_XYdeltaTime = 0;
}
void Dice::handleReleased(int x, int y) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    p_pressed = false;
    m_lastTime.invalidate(); // set touch velocity timer off when released
    m_XYdeltaTime = 0;
}

void Dice::setRunning(bool running)
{
    if (running == m_timer.isActive())
        return;
    if (running)
        m_timer.start();
    else
        m_timer.stop();
    emit runningChanged();
}

void Dice::prep()
{

    // Setup a worker Thread to do the bullet calcs
    m_runner.moveToThread(&m_runnerThread);
    connect(&m_runnerThread, &QThread::finished, &m_runner, &QObject::deleteLater);
    connect(&m_runner, SIGNAL(ready()), this->window(), SLOT(update()) );
    connect(&m_timer, SIGNAL(timeout()), &m_runner, SLOT(runStep()) );
    m_runnerThread.start();
    m_runner.setup(&bullet);

    m_program_dice = new GLProgram();
    qDebug() << "created programs";

    bullet.setupModel();
    emit numDiceChanged(m_numDice);
    qDebug() << "emit numDiceChanged " << m_numDice;
    m_lightTime.start();
    // prep must add and link any shaders
    m_program_dice->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                            SailfishApp::pathTo("dice_vert.glsl.out").toLocalFile());
    m_program_dice->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                            SailfishApp::pathTo("dice_frag.glsl.out").toLocalFile());

    if (! m_program_dice->link()) {
        qDebug() << "Linking failed\n" << m_program_dice->log();
        QList<QOpenGLShader *>::iterator i;
        for (i = m_program_dice->shaders().begin(); i != m_program_dice->shaders().end(); ++i) {
            if ((*i)->isCompiled())
                qDebug() << "Shader compile log: \n" << (*i)->log();
        }

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
            createTextureFromImage(QImage(SailfishApp::pathTo("mer-cube.png")
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


void Dice::render()
{
    handleUse(); // QML input from properties

    // Handle touch events
    if (m_zoomAndSpin)
        handleTouchAsRotation();

    // This should be handled in another thread I think
    // Get the real world information
    QAccelerometerReading *reading = m_sensor.reading();
    Accel a= {reading->x(), reading->y(), reading->z()};
    bullet.setGravity(reading->x(), reading->y(), reading->z());
    int timeDelta_ms = m_lightTime.restart();  /// FIXME this is not bullet time
//    bullet.runStep(timeDelta_ms );
    //for (unsigned int i = 0 ; i < 2 ; i++) { m_dLights[i].update(timeDelta_ms); }
    if (m_fancyLights)
            for (unsigned int i = 0 ; i < 3 ; i++) { m_pLights[i].update(timeDelta_ms); }

    // Prepare to actually draw ///////////////////////////////////////////////////////////
    GLProgram *p = m_program_dice;
    p->bind();
    QMatrix4x4 projMatrix;
    projMatrix.perspective(FOVY, ASPECT, 0.1, 100.0); // The gl port is not rotated so ASPECT is fixed
    p->setUniformValue(p->getU("projMatrixU"), projMatrix);

    QMatrix4x4 viewMatrix;
    viewMatrix = m_cammanager.transform(viewMatrix);
    p->setUniformValue(p->getU("viewMatrixU"), viewMatrix);

    // Setup lighting /////////////////////////////////////////////////////////////////////
    for (unsigned int i = 0 ; i < 2 ; i++) {
        QString pln("directionalLights[%1].");
        p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_dLights[i].light().Base.Color);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_dLights[i].light().Base.AmbientIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_dLights[i].light().Base.DiffuseIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Direction"), m_dLights[i].light().Direction);
    }

    for (unsigned int i = 0 ; i < 3 ; i++) {
        QString pln("pointLights[%1].");
        p->setUniformValue(p->getU(pln.arg(i)+"Base.Color"), m_pLights[i].light().Base.Color);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.AmbientIntensity"), m_pLights[i].light().Base.AmbientIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Base.DiffuseIntensity"), m_pLights[i].light().Base.DiffuseIntensity);
        p->setUniformValue(p->getU(pln.arg(i)+"Position"), m_pLights[i].light().Position);
        //qDebug() << "Setting " << pln.arg(i)+"Position" << "] " << m_pLights[i].light().Position << " in " << p->getU(pln.arg(i)+"Position");
        p->setUniformValue(p->getU(pln.arg(i)+"AConstant"), m_pLights[i].light().AConstant);
        p->setUniformValue(p->getU(pln.arg(i)+"ALinear"), m_pLights[i].light().ALinear);
        p->setUniformValue(p->getU(pln.arg(i)+"AExp"), m_pLights[i].light().AExp);
    }

    p->setUniformValue(p->getU("matSpecularIntensityU"), 2.0f);
    p->setUniformValue(p->getU("specularPowerU"), 32.0f);
    p->setUniformValue(p->getU("eyeWorldPosU"), m_cammanager.at());


    // Bind the texture and use texture unit 0 which contains cube.png
    m_texture->bind();
    p->setUniformValue(p->getU("textureU"), 0);

    // Render the ground/walls /////////////////////////////////////////////////////////////////////

    // TODO

    // Setup Model for cubes /////////////////////////////////////////////////////////////////////
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

    // Update and draw the world
    bullet.renderCubes(p);

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));

    p->release();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Dice::sync()
{
    ++m_frame;
}

DiceRunner::DiceRunner(QObject *parent):
    m_running(false)
{

}

void DiceRunner::setup(Bullet *b) {
    m_bulletTime.start();
    m_workerBullet = b;
}

void DiceRunner::runStep() {
    if (m_running) return;
    m_running = true;
    int timeDelta_ms = m_bulletTime.restart();
    m_workerBullet->runStep(timeDelta_ms );
    emit ready();
    m_running = false;
}
