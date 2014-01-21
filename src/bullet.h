#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector3D>
#include <QList>

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include "glprogram.h"

class Bullet : public QObject
{
    Q_OBJECT
public:
    explicit Bullet(QObject *parent = 0);
    ~Bullet();

    void setupModel();
    void runStep(int ms);
    void report();

    void renderWalls(GLProgram *p);
    void renderCubes(GLProgram *p);
    void addWall(btVector3 normal, float offset);
    void addCube(btVector3 pos, btCollisionShape *shape);

signals:

public slots:
    void setGravity(qreal x, qreal y, qreal z);

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    QList<btCollisionObject*> m_cubes;
    QList<btCollisionObject*>::iterator m_cubes_i;

};

#endif // BULLET_H
