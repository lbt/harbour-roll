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

class DiceRunner : public QObject
{
    Q_OBJECT
public:
    explicit DiceRunner(QObject *parent = 0);

signals:
    void ready();

public slots:
    void runStep();

    void setup(Bullet* b);
private:
    Bullet* m_workerBullet;
    QElapsedTimer m_bulletTime;
    bool m_running;
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
    struct Accel {
        float x;
        float y;
        float z;
    };

    struct Model
    {
        VertexData* vertices;
        GLushort* indices;
    };


    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged)
    Q_PROPERTY(int numDice READ numDice WRITE setNumDice NOTIFY numDiceChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    explicit Dice(QObject *parent = 0);
    qreal x() const { return p_x; }
    void setX(qreal x);
    qreal y() const { return p_y; }
    void setY(qreal y);
    qreal z() const { return p_z; }
    void setZ(qreal z);
    bool running() const { return m_running; }
    void setRunning(bool running);

    void setXYZ(QVector3D v);
    void handleUse();
    void prep();
    void render();

    int numDice() const { return m_numDice; }

signals:
    void xChanged(qreal);
    void yChanged(qreal);
    void zChanged(qreal);
    void runningChanged();
    void pressedChanged();

    void numDiceChanged(int arg);

public slots:
    void sync();
    void handlePressed(int x, int y);
    void handlePositionChanged(int x, int y);
    void handleReleased(int x, int y);
    void useXYZ(QString use);
    void randomiseLights();
    void zoomAndSpin(bool state);
    void setNumDice(int arg);
    void fancyLights(bool state);

private:
    void handleTouchAsRotation();

private:
    QMatrix4x4 m_currMatrix;
    GLuint m_vboIds[4];
    QSGTexture *m_texture;
    GLProgram *m_program_dice;
    
    CameraManager m_cammanager;

    QVector3D m_cameraPos;
    QVector3D m_cameraRot;

    DirectionalLight m_dLights[2];
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
    QTimer m_timer;
    bool m_running;
    bool p_pressed;
    QElapsedTimer m_lastTime;

    QAccelerometer m_sensor;
    int m_numDice;
    bool m_zoomAndSpin;
    bool m_fancyLights;

    Bullet bullet;
    QElapsedTimer m_lightTime;
    QThread m_runnerThread;
    DiceRunner m_runner;
};

#endif // DICE_H
