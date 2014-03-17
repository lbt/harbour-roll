#ifndef DICE_H
#define DICE_H

#include "glitem.h"
#include "glprogram.h"
#include "cameramanager.h"
#include "light.h"
#include "lightmanager.h"

#include "bullet.h"

#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QMutex>
#include <QSGTexture>
#include <QtSensors/QAccelerometer>

#include <QThread>

struct Accel {
    float x;
    float y;
    float z;
};


class DiceRunner : public QObject
{
    Q_OBJECT
public:
    explicit DiceRunner(Bullet *b, QObject *parent = 0);

signals:
    void ready();

public slots:
    void runStep();

    void setup();
    void setRunning(bool state);
    void gravity(bool state);
    void setDebugDraw(bool state);

private:
    QTimer* m_timer;
    Bullet* m_workerBullet;
    QElapsedTimer m_bulletTime;
    QAccelerometer m_sensor;
    bool m_running;
    bool m_gravity;
};

class Dice : public GLItem
{
    Q_OBJECT

    struct VertexData
    {
#define VertexData_0 (0)
        QVector3D position;
#define VertexData_1 (sizeof(QVector3D))
        QVector2D texCoord;
#define VertexData_2 (VertexData_1+sizeof(QVector2D))
        QVector3D normal;
#define VertexData_3 (VertexData_2+sizeof(QVector3D))
    };

    struct Model
    {
        VertexData* vertices;
        GLushort* indices;
    };


    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged)
    Q_PROPERTY(int numDice READ numDice NOTIFY numDiceChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QStringList names READ getNames NOTIFY namesChanged)
    Q_PROPERTY(bool mainLight READ mainLight WRITE setMainLight NOTIFY mainLightChanged)

public:
    explicit Dice(QObject *parent = 0);
    qreal x() const { return p_x; }
    void setX(qreal x);
    qreal y() const { return p_y; }
    void setY(qreal y);
    qreal z() const { return p_z; }
    void setZ(qreal z);
    bool running() const { return m_running; }
    void setXYZ(QVector3D v);
    void handleUse();
    void prep();
    void render();

    int numDice() const { return bullet.numDice(); }
    const QStringList getNames() const;

    bool mainLight() const { return m_mainLight; }

signals:
    void xChanged(qreal);
    void yChanged(qreal);
    void zChanged(qreal);
    void runningChanged();
    void pressedChanged();
    void namesChanged();
    void numDiceChanged(int arg);
    void mainLightChanged(bool arg);

public slots:
    void setRunning(bool running);
    void sync();
    void handlePressed(int x, int y);
    void handlePositionChanged(int x, int y);
    void handleReleased(int x, int y);
    void useXYZ(QString use);
    void randomiseLights();
    void zoomAndSpin(bool state);
    void pickMode(bool state);
    void fancyLights(bool state);
    void gravity(bool state);
    void setDebugDraw(bool state);
    void addDice(QString dice);
    void setMainLight(bool arg);

private:
    void handleTouchAsRotation();

private:
    QMatrix4x4 m_currMatrix;
    GLuint m_vboIds[2];
    GLProgram *m_program_dice;
    
    CameraManager m_cammanager;

    DirectionalLight m_dLights[3];
    PointLight m_pLights[3];

    int m_frame;
    qreal p_x;
    qreal p_y;
    qreal p_z;
    QString m_use;
    qreal m_lastx;
    qreal m_lasty;
    qint64 m_XYdeltaTime;
    qreal m_thread_t;
    Model m_model;
    int m_pcount;
    bool m_running;
    bool p_pressed;
    QElapsedTimer m_lastTime;

    bool m_zoomAndSpin;
    bool m_pickMode;
    bool m_fancyLights;
    bool m_gravity;

    QAccelerometer m_sensor;
    Bullet bullet;
    QElapsedTimer m_lightTime;
    QThread m_runnerThread;
    DiceRunner* m_runner;
    bool m_mainLight;
};

#endif // DICE_H
