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
#include "utils.h"

#include <QDebug>

#define RADIUS 5.0

#define COUNT 1

#define VMIN -1.0
#define VMAX 1.0

#define WIND_R 0.1
#define MAXV 1.5

#define MAXX 2.5
#define MAXY 5.0

Roll::Roll(QObject *parent) :
    GLItem()
  , m_settings(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
               .filePath(QCoreApplication::applicationName())+"/roll.ini",
               QSettings::IniFormat)
  , m_flyCam(NULL)
  , m_followCam(NULL)
  , p_x(0)
  , p_y(0)
  , p_pressed(false)
  , m_zoomAndSpin(false)
  , m_pickMode(true)
  , m_fancyLights(true)
  , m_gravity(true)
  , m_mainLightOn(true)
{
    Q_UNUSED(parent)
    qDebug() << "Making a RollWorld";
    m_world = new RollWorld();
    m_world->setup(); // Post constructor constructor
    qDebug() << "Making a Builder";
    m_builder = new RollBuilder(m_world);
    m_builder->setup();

    // We need to interact with these items:
    m_flyCam = dynamic_cast<CameraFlyer*>(m_world->getCamera("flycam")->motion());
    Q_ASSERT(m_flyCam != NULL);
    m_followCam = dynamic_cast<FollowMotion*>(m_world->getCamera("followcam")->motion());
    Q_ASSERT(m_followCam != NULL);
    m_mainLight = dynamic_cast<DirectionalLight*>(m_world->getLight("main"));
    Q_ASSERT(m_mainLight != NULL);

//    QVariant state(m_settings.value("rollState"));
//    m_world->restore(state.toString());
}

Roll::~Roll()
{
    saveSettings();
}


void Roll::saveSettings() {
    qDebug() << "Saving settings";
//    m_settings.setValue("rollState", m_world->serialise());
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

void Roll::zoomAndSpin(bool state)
{
    m_zoomAndSpin = state;
    // Change of state should also handle press/release
    if (state)
        m_world->setActiveCamera("flycam");
    else
        m_world->setActiveCamera("followcam");
    pickMode(! state);
}

void Roll::pickMode(bool state)
{
    m_pickMode = state;
    // Change of state should also handle press/release
//    if (!state)
//        m_world->release();
}
void Roll::setCameraPos(qreal x, qreal y, qreal z)
{
    m_flyCam->lookAt(QVector3D(x,y,z), QVector3D(),
                     QVector3D(0, 0, 1));
}

void Roll::fancyLights(bool state)
{
    if (m_fancyLights == state) return;
    m_fancyLights = state;
    for (auto l : m_world->getLights()) {
        // our motion managers may be orbiters
//        MotionOrbiter* mo = dynamic_cast<MotionOrbiter*>(l->motion());
//        if (mo) mo->active(state);
    }
}

void Roll::setMainLight(bool arg)
{
    if (m_mainLightOn != arg) {
        m_mainLightOn = arg;
        emit mainLightChanged(arg);
    }
    if (arg) {
        // Ensure that the first throw has a visible dlight
        m_mainLight->setBaseLight(QVector3D(1.0, 1.0, 1.0), 0.4, 0.8);
        m_mainLight->setDirectionalLight(QVector3D(-1, 1, 4).normalized());
    } else {
        m_mainLight->randomise();
    }
}

void Roll::randomiseLights()
{
    for (auto l : m_world->getLights()) {
        l->randomise();
    }
    if (m_mainLightOn) {
        m_mainLight->setBaseLight(QVector3D(1.0, 1.0, 1.0), 0.4, 0.8);
        m_mainLight->setDirectionalLight(QVector3D(-1, 1, 4).normalized());
    }
}

void Roll::gravity(bool state)
{
    m_gravity = state;
    m_world->gravity(state);
}

void Roll::setDebugDraw(bool state)
{
    m_world->setDebugDraw(state);
}
void Roll::setRunning(bool running) {
    m_world->setRunning(running);
}

void Roll::useTrack(QString track)
{
    Q_UNUSED(track)
//    m_world->useTrack(track);
}

const QStringList Roll::getNames() const {
    QStringList l = m_world->getTrackNames();
    l.sort();
    return l;
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
        m_flyCam->touch(p_x, p_y);
    } else {
        //    if (m_pickMode) { // Bullet knows nothing about the screen. It needs world info:
//        float fx = ((float)x/(float)m_cammanager.screenWidth()  - 0.5f) * 2.0f; // [0,xxx] -> [-1,1]
//        float fy = (0.5f - (float)y/(float)m_cammanager.screenHeight()) * 2.0f; // [yyy,0] -> [-1,1] (screen is inverted compared to GL)
        //m_world->touch(fx, fy, m_cammanager.projViewMatrix(), m_cammanager.forward());
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
        m_flyCam->release();
    qDebug()<< "Camera at " << m_flyCam->at() << " looking " << m_flyCam->forward();
    //    if (m_pickMode)
    //m_world->release();

}


QQuickWindow* global_hack_window;
///////////////////////////////////////////////////////
/// \brief Roll::prep
///
/// called from the render thread
void Roll::prep()
{
    global_hack_window = window(); // This is until we get 5.2 and QOpenGLTextures
    qDebug() << "roll Prep";

    // and then prepare any one-time data like VBOs
    connect(m_world, SIGNAL(stepReady()), this->window(), SLOT(update()) );
    m_builder->setupGL();
    qDebug() << "done builder setupGL";
    m_world->setupGL();
    qDebug() << "done world setupGL";
    m_world->start();
}


// This is the world render routine.

///////////////////////////////////////////////////////
/// \brief Roll::render
///
/// called from the render thread
void Roll::render()
{
    if (m_zoomAndSpin) {
        if (p_pressed) {
            m_flyCam->touch(p_x, p_y);
        }
    } else {
    }

    // Draw the world
    m_world->render();

}
