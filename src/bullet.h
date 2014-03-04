#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QList>
#include <QHash>
#include <QList>
#include <QMutex>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/LinearMath/btIDebugDraw.h>

#include <stdio.h>

#include "glprogram.h"
#include "bimesh.h"


class Bullet : public QObject, public btIDebugDraw
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
    explicit Bullet(QObject *parent = 0);
    ~Bullet();

    virtual void setupModel();
    void runStep(int ms);
    void report();

    void renderWalls(GLProgram *p);
    void render(GLProgram *p, QMatrix4x4 projViewMatrix);
    void addWall(btVector3 normal, float offset);

signals:

public slots:
    void setGravity(qreal x, qreal y, qreal z);
    void kick();
    void setNumCubes(int n);

protected:
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    void addCube(btVector3 pos);
    void addDice(QString name, btVector3 pos);
    void loadDice();

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;

    QMutex m_cubeMutex;
    QList<btCollisionObject*> m_cubes;
    QList<btCollisionObject*>::iterator m_cubes_i;
    btCollisionShape* m_cubeShape;
    QHash<QString, btCollisionShape*> m_diceShape;
    BiMeshContainer* m_meshes;
    QHash<Color, QList<Line> > *m_worldLines;
    typedef QHash<Color, QList<Line> >::const_iterator m_worldLines_iter_type;
    typedef QList<Line>::const_iterator m_worldLines_list_iter_type;
    QVector<QVector3D> m_qlinepoints;


    // Below here is the functionality needed to be a btIDebugDraw
public:
    void setup(GLProgram *p); // setup GL stuff
    void drawLine(const btVector3 &  	from,
                  const btVector3 &  	to,
                  const btVector3 &  	color);
    void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    void	reportErrorWarning(const char* warningString);
    void	draw3dText(const btVector3& location,const char* textString);
    void	setDebugMode(int debugMode);
    int		getDebugMode() const;
    void render2(GLProgram *p, QMatrix4x4 projViewMatrix);
private:
    GLProgram* m_program_debug;
    int m_debug_mode;
};

#endif // BULLET_H
