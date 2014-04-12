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
  , p_x(0)
  , p_y(0)
  , p_pressed(false)
  , m_zoomAndSpin(false)
  , m_pickMode(true)
  , m_gravity(true)
  , m_mainLightOn(true)
{
    Q_UNUSED(parent)
    qDebug() << "Making a RollWorld";
    m_world = new RollWorld();
    m_world->setup(); // Post constructor constructor
    qDebug() << "Making a Builder";
    m_builder = new WorldBuilder(m_world);
    m_builder->setup();

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
//    if (!state)
//        m_cammanager.reset();
    pickMode(! state);
    m_world->fly(state);
}

void Roll::pickMode(bool state)
{
    m_pickMode = state;
    // Change of state should also handle press/release
//    if (!state)
//        m_world->release();
}

void Roll::fancyLights(bool state)
{
    if (m_fancyLights == state) return;
    m_fancyLights = state;
    for (auto l : m_world->getLights()) {
        l->lightManager.active(state);
    }
}

void Roll::setMainLight(bool arg)
{
    if (m_mainLightOn != arg) {
        m_mainLightOn = arg;
        emit mainLightChanged(arg);
    }
    _DirectionalLight light;
    if (arg) {
        // Ensure that the first throw has a visible dlight
        light.Base.Color = QVector3D(1.0, 1.0, 1.0);
        light.Base.AmbientIntensity=0.4;
        light.Base.DiffuseIntensity=0.8;
        light.Direction = QVector3D(-1, 1, 4).normalized();
        m_mainLight->set(light);
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
        _DirectionalLight light;
        // Ensure that the first throw has a visible dlight
        light.Base.Color = QVector3D(1.0, 1.0, 1.0);
        light.Base.AmbientIntensity=0.4;
        light.Base.DiffuseIntensity=0.8;
        light.Direction = QVector3D(-1, 1, 4).normalized();
        m_mainLight->set(light);
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
        m_cammanager.touch(p_x, p_y);
    } else {
        //    if (m_pickMode) { // Bullet knows nothing about the screen. It needs world info:
//        float fx = ((float)x/(float)m_cammanager.screenWidth()  - 0.5f) * 2.0f; // [0,xxx] -> [-1,1]
//        float fy = (0.5f - (float)y/(float)m_cammanager.screenHeight()) * 2.0f; // [yyy,0] -> [-1,1] (screen is inverted compared to GL)
        qDebug()<< "Camera at " << m_cammanager.at();
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
        m_cammanager.release();
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

    QVariant state(m_settings.value("rollState"));
    m_world->restore(state.toString());

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
            m_cammanager.touch(p_x, p_y);
        }
        m_cammanager.updatePosition();
    } else {
//        m_cammanager.follow(m_world->getFollowInfo());
    }
//    bool m_follow=true;
//    if (m_follow) {
//        m_cammanager.follow(bullet.getFollowInfo());
//    }

    // Draw the world
    m_world->render();

}
