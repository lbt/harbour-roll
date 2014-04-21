#ifndef ROLL_H
#define ROLL_H

#include "glitem.h"
#include "glprogram.h"
#include "rollworld.h"
#include "cameraflyermotion.h"
#include "followmotion.h"
#include "light.h"
#include "orbitmotion.h"
#include "worlditem.h"

#include "rollbuilder.h"

#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QMutex>
#include <QSGTexture>
#include <QtSensors/QAccelerometer>
#include <QSettings>

#include <QThread>

struct Accel {
    float x;
    float y;
    float z;
};


class Roll : public GLItem
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QStringList names READ getNames NOTIFY namesChanged)
    Q_PROPERTY(bool mainLight READ mainLight WRITE setMainLight NOTIFY mainLightChanged)

public:
    explicit Roll(QObject *parent = 0);
    ~Roll();

    qreal x() const { return p_x; }
    void setX(qreal x);
    qreal y() const { return p_y; }
    void setY(qreal y);
    qreal z() const { return p_z; }
    void setZ(qreal z);
    bool running() const { return m_running; }
    void setXYZ(QVector3D v);
    void prep();
    void render();

    const QStringList getNames() const;

    bool mainLight() const { return m_mainLightOn; }

signals:
    void xChanged(qreal);
    void yChanged(qreal);
    void zChanged(qreal);
    void runningChanged();
    void pressedChanged();
    void namesChanged();
    void mainLightChanged(bool arg);

public slots:
    void setRunning(bool running);
    void handlePressed(int x, int y);
    void handlePositionChanged(int x, int y);
    void handleReleased(int x, int y);
    void randomiseLights();
    void zoomAndSpin(bool state);
    void pickMode(bool state);
    void fancyLights(bool state);
    void gravity(bool state);
    void setDebugDraw(int mode);
    void useTrack(QString track);
    void setMainLight(bool arg);

    void saveSettings();
    void setCameraPos(qreal x, qreal y, qreal z);
    void setCamera(QString camera);
private:
    void handleTouchAsRotation();

private:
    QSettings m_settings;
    
    CameraFlyerMotion* m_flyCam;
    FollowMotion* m_followCam;

    qreal p_x;
    qreal p_y;
    qreal p_z;
    qreal m_lastx;
    qreal m_lasty;
    qint64 m_XYdeltaTime;
    int m_pcount;
    bool m_running;
    bool p_pressed;
    QElapsedTimer m_lastTime;

    bool m_zoomAndSpin;
    bool m_pickMode;
    bool m_fancyLights;
    bool m_gravity;

    RollBuilder* m_builder;
    RollWorld* m_world;
    QElapsedTimer m_lightTime;
    bool m_mainLightOn;
    DirectionalLight* m_mainLight;
};

#endif // ROLL_H
