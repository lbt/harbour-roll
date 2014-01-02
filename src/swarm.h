#ifndef SWARM_H
#define SWARM_H

#include "glitem.h"
#include "gparticle.h"
#include <QTimer>

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
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    explicit Swarm(QObject *parent = 0);
    float rnd(float max);
    qreal t() const { return m_t; }
    void setT(qreal t);
    bool running() const { return m_running; }
    void setRunning(bool running);

    void prep();
    void render();

signals:
    void tChanged();
    void runningChanged();

public slots:
    void sync();


private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    int m_frame;
    GLuint m_vboIds[2];
    qreal m_t;
    qreal m_thread_t;
    Model m_model;
    QList<GParticle> m_swarm;
    int m_pcount;
    QTimer m_timer;
    bool m_running;
};

#endif // SWARM_H
