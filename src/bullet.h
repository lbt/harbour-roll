#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector3D>
#include <QList>
#include <QMap>
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
    QMap<QString, btCollisionShape*> m_diceShape;
    BiMesh* m_meshes;

    // Below here is the functionality needed to be a btIDebugDraw
public:
    void setup(); // setup GL stuff
    void drawLine(const btVector3 &  	from,
                  const btVector3 &  	to,
                  const btVector3 &  	color);
    void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    void	reportErrorWarning(const char* warningString);
    void	draw3dText(const btVector3& location,const char* textString);
    void	setDebugMode(int debugMode);
    int		getDebugMode() const;
private:
    GLProgram* m_program_debug;
    int m_debug_mode;
};

#endif // BULLET_H
