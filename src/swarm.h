#ifndef SWARM_H
#define SWARM_H

#include "glitem.h"
#include "gparticle.h"
#include <QTimer>
#include <QElapsedTimer>
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

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressedChanged)

public:
    explicit Swarm(QObject *parent = 0);
    float rnd(float max);
    qreal t() const { return m_t; }
    void setT(qreal t);
    qreal x() const { return m_x; }
    void setX(qreal x);
    qreal y() const { return m_y; }
    void setY(qreal y);
    bool running() const { return m_running; }
    void setRunning(bool running);
    bool pressed() const { return m_pressed; }
    void setPressed(bool pressed);

    void prep();
    void render();

signals:
    void tChanged();
    void xChanged();
    void yChanged();
    void runningChanged();
    void pressedChanged();

public slots:
    void sync();


private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    QMatrix4x4 m_currMatrix;
    int m_frame;
    GLuint m_vboIds[2];
    qreal m_t;
    qreal m_x;
    qreal m_y;
    qreal m_lastx;
    qreal m_lasty;
    qreal m_thread_t;
    Model m_model;
    QList<GParticle> m_swarm;
    int m_pcount;
    QTimer m_timer;
    bool m_running;
    bool m_pressed;

    GParticle::Wind m_wind;
    GParticle::Wind m_lastWind;
    QElapsedTimer m_lastTime;

};

#endif // SWARM_H
