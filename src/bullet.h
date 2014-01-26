#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector3D>
#include <QList>
#include <QMutex>

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include "glprogram.h"

class Bullet : public QObject
{
    Q_OBJECT
public:
    explicit Bullet(QObject *parent = 0);
    ~Bullet();

    virtual void setupModel();
    void runStep(int ms);
    void report();

    void renderWalls(GLProgram *p);
    void renderCubes(GLProgram *p);
    void addWall(btVector3 normal, float offset);
    void addCube(btVector3 pos);

signals:

public slots:
    void setGravity(qreal x, qreal y, qreal z);

    void kick();

    void setNumCubes(int n);
protected:
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

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

};

#endif // BULLET_H
