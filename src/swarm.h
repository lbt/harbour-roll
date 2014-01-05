#ifndef SWARM_H
#define SWARM_H

#include "glitem.h"
#include "gparticle.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QMutex>
#include <QSGTexture>
#include <QtSensors/QAccelerometer>


class Swarm : public GLItem
{
    Q_OBJECT

    struct VertexData
    {
#define VertexData_0 (0)
        QVector3D position;
#define VertexData_1 (sizeof(QVector3D))
        QVector2D texCoord;
#define VertexData_2 (Vertex_Data_2+sizeof(QVector2D))
    };

    struct Model
    {
        VertexData* vertices;
        GLushort* indices;
    };

    Q_PROPERTY(int numParticles READ numParticles WRITE setNumParticles NOTIFY numParticlesChanged)
    Q_PROPERTY(qreal depth READ depth WRITE setDepth NOTIFY depthChanged)
    Q_PROPERTY(int orientationInDegrees READ orientationInDegrees WRITE setOrientationInDegrees NOTIFY orientationInDegreesChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    explicit Swarm(QObject *parent = 0);
    float rnd(float max);
    qreal numParticles() const { return p_numParticles; }
    void setNumParticles(int numParticles);
    qreal depth() const { return p_depth; }
    void setDepth(qreal depth);
    int orientationInDegrees() const { return p_orientationInDegrees; }
    void setOrientationInDegrees(int orientationInDegrees);
    qreal x() const { return p_x; }
    void setX(qreal x);
    qreal y() const { return p_y; }
    void setY(qreal y);
    bool running() const { return m_running; }
    void setRunning(bool running);

    void prep();
    void render();

signals:
    void numParticlesChanged();
    void depthChanged();
    void orientationInDegreesChanged();
    void xChanged();
    void yChanged();
    void runningChanged();
    void pressedChanged();

public slots:
    void sync();
    void handlePressed(int x, int y);
    void handlePositionChanged(int x, int y);
    void handleReleased(int x, int y);
    void handleOrientationChanged(int orientation);


private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    GLuint m_modelCol_U;
    QMatrix4x4 m_currMatrix;
    GLuint m_vboIds[2];
    QSGTexture *m_texture;
    QOpenGLShaderProgram *m_programPointer;

    int m_frame;
    int m_orientationInDegrees;
    int p_orientationInDegrees;
    int p_numParticles;
    qreal p_depth;
    qreal p_x;
    qreal p_y;
    qreal m_lastx;
    qreal m_lasty;
    qint64 m_XYdeltaTime;
    qreal m_thread_t;
    Model m_model;
    QList<GParticle> m_swarm;
    QMutex m_swarmMutex;
    int m_pcount;
    QTimer m_timer;
    bool m_running;
    bool p_pressed;
    GParticle::Wind m_wind;
    GParticle::Wind m_lastWind;
    QElapsedTimer m_lastTime;

    QAccelerometer m_sensor;
};

#endif // SWARM_H
