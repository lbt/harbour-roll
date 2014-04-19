#ifndef WORLDDEBUGDRAWER_H
#define WORLDDEBUGDRAWER_H

#include <QObject>
#include <QMutex>
#include "glprogram.h"
#include "bullet/LinearMath/btIDebugDraw.h"

class World;

class WorldDebugDrawer : public QObject, public btIDebugDraw
{
    Q_OBJECT
public:
    struct Line {
        const QVector3D from;
        const QVector3D to;
    };

    class Color {
    public:
        Color(QVector4D c) { m_c = c; }
        Color() {}
        Color(const Color &other) { m_c = other.m_c; }
        ~Color() {}
        Color &operator=(const Color &other);

    public:
        QVector4D m_c;
    };

public:
    explicit WorldDebugDrawer(World* parent);

    void setupGL(); // setup GL stuff
    void newFrame(); // Prepare to draw a new frame
    void render(QMatrix4x4 projViewMatrix);

    // Implement the interface
    void drawLine(const btVector3 &  	from,
                  const btVector3 &  	to,
                  const btVector3 &  	color);
    void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    void reportErrorWarning(const char* warningString);
    void draw3dText(const btVector3& location,const char* textString);
    void setDebugMode(int debugMode) { m_debug_mode = debugMode; }
    int	 getDebugMode() const { return m_debug_mode; }
signals:

public slots:

private:
    GLProgram* m_program_debug;
    int m_debug_mode;
    QHash<Color, QList<Line> > *m_worldLines;
    typedef QHash<Color, QList<Line> >::const_iterator m_worldLines_iter_type;
    typedef QList<Line>::const_iterator m_worldLines_list_iter_type;
    QVector<QVector3D> m_qlinepoints;
    QMutex m_worldLineMutex;

};
uint qHash(WorldDebugDrawer::Color c);
inline bool operator==(const WorldDebugDrawer::Color &c1, const WorldDebugDrawer::Color &c2) { return c1.m_c == c2.m_c; }

#endif // WORLDDEBUGDRAWER_H
