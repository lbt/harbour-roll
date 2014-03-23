#include "roll.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QtGui/QOpenGLContext>
#include <QImage>
#include <QVector3D>

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>

#include <sailfishapp.h>

#include "math.h"
#include <bullet/LinearMath/btIDebugDraw.h>

#include <QDebug>

#define RADIUS 5.0
#define PI 3.1415926535
#define PI2 6.283185307

#define COUNT 1

#define TICK 17
#define VMIN -1.0
#define VMAX 1.0

#define WIND_R 0.1
#define MAXV 1.5

#define MAXX 2.5
#define MAXY 5.0

namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

Roll::Roll(QObject *parent) :
    GLItem()
  , m_frame(0)
  , p_x(0)
  , p_y(0)
  , m_thread_t(0)
  , p_pressed(false)
  , m_zoomAndSpin(false)
  , m_pickMode(true)
  , m_gravity(true)
  , m_mainLight(true)
  , m_settings(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
               .filePath(QCoreApplication::applicationName())+"/roll.ini",
               QSettings::IniFormat)
{
    Q_UNUSED(parent)
    m_sensor.start();

    for (int i=0; i<3; i++) { m_pLights[i].lightManager.setScale(QVector3D(4.0, 5.0, 4.0)); }
    randomiseLights();

    connect(&bullet, SIGNAL(numWorldObjectsChanged(int)), this, SIGNAL(numDiceChanged(int)));
}

Roll::~Roll()
{
    saveSettings();
}


void Roll::saveSettings() {
    qDebug() << "Saving settings";
    m_settings.setValue("rollState", bullet.serialise());
}

void Roll::setX(qreal x)
{
    if (x == p_x)
        return;
    p_x = x;
    emit xChanged(x);
}
void Roll::setY(qreal y)
{
    if (y == p_y)
        return;
    p_y = y;
    emit yChanged(y);
}
void Roll::setZ(qreal z)
{
    if (z == p_z)
        return;
    p_z = z;
    emit zChanged(z);
}
void Roll::setXYZ(QVector3D v) {
    setX(v.x());
    setY(v.y());
    setZ(v.z());
}

void Roll::useXYZ(QString use) {
    m_use = use;
}


void Roll::zoomAndSpin(bool state)
{
    m_zoomAndSpin = state;
    // Change of state should also handle press/release
    if (!state)
        m_cammanager.reset();
    pickMode(! state);
    QMetaObject::invokeMethod(m_runner, "fly", Qt::QueuedConnection, Q_ARG(bool, state));
}

void Roll::pickMode(bool state)
{
    m_pickMode = state;
    // Change of state should also handle press/release
    if (!state)
        bullet.release();
}

void Roll::fancyLights(bool state)
{
    m_fancyLights = state;
}

void Roll::setMainLight(bool arg)
{
    if (m_mainLight != arg) {
        m_mainLight = arg;
        emit mainLightChanged(arg);
    }
    _DirectionalLight light;
    if (arg) {
        // Ensure that the first throw has a visible dlight
        light.Base.Color = QVector3D(1.0, 1.0, 1.0);
        light.Base.AmbientIntensity=0.4;
        light.Base.DiffuseIntensity=0.8;
        light.Direction = QVector3D(-1, 1, 4).normalized();
        m_dLights[0].set(light);
    } else {
        m_dLights[0].randomise();
    }
}

void Roll::randomiseLights()
{
    for (int i=0; i<2; i++) { m_dLights[i].randomise(); }
    for (int i=0; i<3; i++) { m_pLights[i].randomise(); }
    if (m_mainLight) {
        _DirectionalLight light;
        // Ensure that the first throw has a visible dlight
        light.Base.Color = QVector3D(1.0, 1.0, 1.0);
        light.Base.AmbientIntensity=0.4;
        light.Base.DiffuseIntensity=0.8;
        light.Direction = QVector3D(-1, 1, 4).normalized();
        m_dLights[0].set(light);
    }
}

void Roll::gravity(bool state)
{
    m_gravity = state;
    QMetaObject::invokeMethod(m_runner, "gravity", Qt::QueuedConnection, Q_ARG(bool, state));
}

void Roll::setDebugDraw(bool state)
{
    QMetaObject::invokeMethod(m_runner, "setDebugDraw", Qt::QueuedConnection, Q_ARG(bool, state));
}

void Roll::addDice(QString dice)
{
    bullet.addDice(dice);
}


void Roll::handleUse() {
}

void Roll::handlePositionChanged(int x, int y)
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

void Roll::handlePressed(int x, int y) {
    p_pressed = true;
    m_lastTime.start(); // set touch velocity timer on when touched
    p_x = x;
    p_y = y;
    m_XYdeltaTime = 0;

    // Decide who gets events. Note that changing mode whilst p_pressed should stop one and start another
    if (m_zoomAndSpin) {
        m_cammanager.touch(p_x, p_y);
    } else {
        //    if (m_pickMode) { // Bullet knows nothing about the screen. It needs world info:
        float fx = ((float)x/(float)m_cammanager.screenWidth()  - 0.5f) * 2.0f; // [0,xxx] -> [-1,1]
        float fy = (0.5f - (float)y/(float)m_cammanager.screenHeight()) * 2.0f; // [yyy,0] -> [-1,1] (screen is inverted compared to GL)
        qDebug()<< "Camera at " << m_cammanager.at();
        bullet.touch(fx, fy, m_cammanager.projViewMatrix(), m_cammanager.forward());
    }
    //    }
}
void Roll::handleReleased(int x, int y) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    p_pressed = false;
    m_lastTime.invalidate(); // set touch velocity timer off when released
    m_XYdeltaTime = 0;

    if (m_zoomAndSpin)
        m_cammanager.release();
    //    if (m_pickMode)
    bullet.release();

}

void Roll::setRunning(bool running)
{
    m_running = running;
    QMetaObject::invokeMethod(m_runner, "setRunning", Qt::QueuedConnection, Q_ARG(bool, running));
}

extern QQuickWindow* global_hack_window;
void Roll::prep()
{
    global_hack_window = window(); // This is until we get 5.2 and QOpenGLTextures
    qDebug() << "roll Prep";
    // Setup a worker Thread to do the bullet calcs
    m_runner = new RollRunner(&bullet);
    m_runner->moveToThread(&m_runnerThread);
    connect(&m_runnerThread, &QThread::finished, m_runner, &QObject::deleteLater);
    connect(&m_runnerThread, &QThread::started, m_runner, &RollRunner::setup);
    connect(m_runner, SIGNAL(ready()), this->window(), SLOT(update()) );
    m_runnerThread.start();

    m_program_dice = new GLProgram(SailfishApp::pathTo("roll_vert.glsl.out"), SailfishApp::pathTo("roll_frag.glsl.out"));
    //    m_program_dice = new GLProgram(SailfishApp::pathTo("roll_vert.glsl.out"), SailfishApp::pathTo("debug_frag.glsl"));
    qDebug() << "created programs";

    QVariant state(m_settings.value("rollState"));
    bullet.setupModel(state.toString());
    emit namesChanged();
    emit numDiceChanged(numDice());
    m_lightTime.start();

    // and then prepare any one-time data like VBOs
    glGenBuffers(2, m_vboIds);

#define AXIS_LEN 1
    QVector3D axes[] = {
        QVector3D(-AXIS_LEN, 0, 0), QVector3D(AXIS_LEN, 0, 0),
        QVector3D(0, -AXIS_LEN, 0), QVector3D(0, AXIS_LEN, 0),
        QVector3D(0, 0, -AXIS_LEN), QVector3D(0, 0, AXIS_LEN),
    };
    GLushort axesorder[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5};

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(QVector3D), axes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10 * sizeof(GLushort), axesorder, GL_STATIC_DRAW);

    // Load cube.png image
    glEnable(GL_TEXTURE_2D);

    bullet.setupGL(m_program_dice);
}


// This is the world render routine.

void Roll::render()
{
    QElapsedTimer t;
    t.start();

    handleUse(); // QML input from properties
    if (m_zoomAndSpin) {
        if (p_pressed) {
            m_cammanager.touch(p_x, p_y);
        }
        m_cammanager.updatePosition();
    }

    int timeDelta_ms = m_lightTime.restart();  /// FIXME this is not bullet time. Also FIXME and update in the DiceRunner thread
    //for (unsigned int i = 0 ; i < 2 ; i++) { m_dLights[i].update(timeDelta_ms); }
    if (m_fancyLights)
        for (unsigned int i = 0 ; i < 3 ; i++) { m_pLights[i].update(timeDelta_ms); }

    // Prepare to actually draw ///////////////////////////////////////////////////////////
    GLProgram *p = m_program_dice;
    p->bind();

    QMatrix4x4 projViewMatrix = m_cammanager.projViewMatrix();

    p->setUniformValue(p->getU("projViewMatrixU"), projViewMatrix);


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


    p->setUniformValue(p->getU("colU"), QVector4D(0.6, 0.7, 0.8, 1.0));
    p->setUniformValue(p->getU("Glow"), QVector4D(0.0, 0.0, 0.0, 1.0));

    // Render the ground/walls /////////////////////////////////////////////////////////////////////

    // TODO

    // Setup Model for cubes /////////////////////////////////////////////////////////////////////

    // Draw the world
    bullet.render(p, projViewMatrix);

    if (bullet.getDebugMode()) {
        // Draw the lights
        for (unsigned int i = 0 ; i < 2 ; i++) {
            m_dLights[i].debugRender(projViewMatrix);
        }
        for (unsigned int i = 0 ; i < 3 ; i++) {
            m_pLights[i].debugRender(projViewMatrix);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //    qDebug() << "Render took " << t.elapsed();

}

const QStringList Roll::getNames() const {
    QStringList l = bullet.getNames();
    l.sort();
    return l;
}

void Roll::sync()
{
    ++m_frame;
}

RollRunner::RollRunner(Bullet *b, QObject *parent):
    m_running(false)
  , m_gravity(true)
  , m_fly(false)
{
    m_workerBullet = b;
}

void RollRunner::setup() {
    m_bulletTime.start();
    m_sensor.start();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(runStep()) );
    m_timer->setInterval(TICK);;
    m_timer->start();
}

void RollRunner::setRunning(bool running){
    if (m_running == running) return;

    m_running = running;
    if (running) {
        m_timer->start();
    } else {
        m_timer->stop();
    }
}

void RollRunner::gravity(bool state)
{
    m_gravity = state;
}

void RollRunner::setDebugDraw(bool state)
{
    if (state)
        m_workerBullet->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    else
        m_workerBullet->setDebugMode(0);
}

void RollRunner::runStep() {
    if (m_gravity) {
        if (!m_fly) {
            QAccelerometerReading *reading = m_sensor.reading();
            m_workerBullet->setGravity(reading->x(), reading->y(), reading->z());
        }
    } else {
        m_workerBullet->setGravity(0, 0, 0);
    }
    //    qDebug() << "tick";
    int timeDelta_ms = m_bulletTime.restart();
    m_workerBullet->runStep(timeDelta_ms );
    emit ready();
}
